#!/usr/bin/env bash
# ESP32-S3 Firmware Build Script
# Builds the project and merges bootloader + partitions + boot_app0 + firmware
# into a single flashable binary, versioned from the latest git tag.
#
# Usage:
#   ./build_firmware.sh                  # version resolved from git tags
#   VERSION=1.2.3 ./build_firmware.sh    # force a version
#   PIO_ENV=my-env ./build_firmware.sh   # override PlatformIO environment
#   INJECT_VERSION=0 ./build_firmware.sh # do not pass -D FIRMWARE_VERSION
#   MARK_DIRTY=1 ./build_firmware.sh     # append "-dirty" on an unclean tree
#
# Run from the project root directory.

set -Eeuo pipefail

# ---------------------------------------------------------------- config ----
PROJECT_NAME="MatrixClock"
CHIP="esp32s3"
FLASH_SIZE="16MB"          # ESP32-S3-N16R8 has 16MB flash
FLASH_MODE="dio"
FLASH_FREQ="80m"
DIST_DIR="${DIST_DIR:-dist}"
INJECT_VERSION="${INJECT_VERSION:-1}"
MARK_DIRTY="${MARK_DIRTY:-0}"   # 1 = append "-dirty" when the tree is not clean

# Colors (disabled when stdout is not a terminal, e.g. in CI logs)
if [ -t 1 ]; then
    RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; NC='\033[0m'
else
    RED=''; GREEN=''; YELLOW=''; NC=''
fi

die()  { echo -e "${RED}Error: $*${NC}" >&2; exit 1; }
warn() { echo -e "${YELLOW}Warning: $*${NC}" >&2; }
ok()   { echo -e "${GREEN}✓${NC} $*"; }

# ----------------------------------------------------------- environment ----
[ -f platformio.ini ] || die "platformio.ini not found. Run this script from the project root."
command -v pio >/dev/null 2>&1 || die "pio command not found. Please install PlatformIO Core."

# Read the PlatformIO environment name from platformio.ini unless overridden,
# so renaming the board does not silently break this script.
if [ -z "${PIO_ENV:-}" ]; then
    PIO_ENV=$(sed -n 's/^\[env:\(.*\)\][[:space:]]*$/\1/p' platformio.ini | head -n1)
fi
[ -n "$PIO_ENV" ] || die "No [env:...] section found in platformio.ini"

BUILD_DIR=".pio/build/${PIO_ENV}"

# --------------------------------------------------------------- version ----
# Version rules:
#   HEAD exactly on a tag        -> 1.2.3             (release)
#   HEAD ahead of the latest tag -> 1.2.3-4-gabc1234  (snapshot)
#   no tag at all                -> 0.0.0-gabc1234
#   not a git repository         -> 0.0.0-nogit
#
# Uncommitted changes only produce a warning. Set MARK_DIRTY=1 to append
# "-dirty" to the version instead, which keeps a modified build from ever
# being published under a clean release number.
resolve_version() {
    if ! git rev-parse --git-dir >/dev/null 2>&1; then
        warn "Not a git repository, falling back to 0.0.0-nogit"
        echo "0.0.0-nogit"
        return
    fi

    local dirty="" tag desc
    if [ "$MARK_DIRTY" = "1" ] && [ -n "$(git status --porcelain 2>/dev/null)" ]; then
        dirty="-dirty"
    fi

    if tag=$(git describe --tags --exact-match HEAD 2>/dev/null); then
        # Exactly on a tag: strip a leading "v" and use the tag as the version
        echo "${tag#v}${dirty}"
        return
    fi

    if git describe --tags --abbrev=0 >/dev/null 2>&1; then
        # e.g. "v1.2.3-4-gabc1234" -> "1.2.3-4-gabc1234"
        desc=$(git describe --tags --long 2>/dev/null)
        echo "${desc#v}${dirty}"
        return
    fi

    echo "0.0.0-g$(git rev-parse --short HEAD 2>/dev/null || echo unknown)${dirty}"
}

VERSION="${VERSION:-$(resolve_version)}"

case "$VERSION" in
    *dirty*|*-g*|*nogit*) IS_RELEASE=0 ;;
    *)                    IS_RELEASE=1 ;;
esac

# Keep the version safe to embed in a filename
SAFE_VERSION=$(printf '%s' "$VERSION" | tr -c 'A-Za-z0-9._-' '_')
OUTPUT_FILE="${DIST_DIR}/${PROJECT_NAME}_v${SAFE_VERSION}.bin"

# --------------------------------------------------------------- esptool ----
# esptool v4 uses snake_case subcommands, v5 uses kebab-case.
ESPTOOL=""
for candidate in esptool esptool.py; do
    if command -v "$candidate" >/dev/null 2>&1; then ESPTOOL="$candidate"; break; fi
done
[ -n "$ESPTOOL" ] || die "esptool not found. Install it with: pip install esptool"

ESPTOOL_MAJOR=$("$ESPTOOL" version 2>/dev/null | grep -oE '[0-9]+' | head -n1 || true)
ESPTOOL_MAJOR="${ESPTOOL_MAJOR:-4}"
if [ "$ESPTOOL_MAJOR" -ge 5 ]; then
    CMD_MERGE="merge-bin"
else
    CMD_MERGE="merge_bin"
fi

# ------------------------------------------------------------- boot_app0 ----
# Honour PLATFORMIO_CORE_DIR instead of assuming ~/.platformio
PIO_CORE_DIR="${PLATFORMIO_CORE_DIR:-$HOME/.platformio}"
BOOT_APP0_PATH="${PIO_CORE_DIR}/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin"
if [ ! -f "$BOOT_APP0_PATH" ]; then
    # Some framework releases ship it under a version-suffixed package directory
    BOOT_APP0_PATH=$(find "${PIO_CORE_DIR}/packages" -name boot_app0.bin -print -quit 2>/dev/null || true)
fi

# ------------------------------------------------------------------ info ----
echo -e "${GREEN}=== ESP32-S3 Build Firmware Tool ===${NC}\n"
echo "Project:    $PROJECT_NAME"
echo "Version:    $VERSION"
echo "PIO env:    $PIO_ENV"
echo "Flash size: $FLASH_SIZE"
echo "esptool:    $ESPTOOL (major $ESPTOOL_MAJOR)"
echo "Output:     $OUTPUT_FILE"
[ "$IS_RELEASE" -eq 1 ] || warn "Not a clean tagged build (snapshot commit)."
if [ "$MARK_DIRTY" != "1" ] && [ -n "$(git status --porcelain 2>/dev/null)" ]; then
    warn "Working tree has uncommitted changes - the binary does not match ${VERSION} exactly."
fi
echo ""

# ----------------------------------------------------------------- build ----
# Make the version visible to the firmware itself so the settings/About page
# and the file name can never disagree. Guard it in code with:
#     #ifndef FIRMWARE_VERSION
#     #define FIRMWARE_VERSION "dev"
#     #endif
if [ "$INJECT_VERSION" = "1" ]; then
    export PLATFORMIO_BUILD_FLAGS="${PLATFORMIO_BUILD_FLAGS:-} -DFIRMWARE_VERSION=\\\"${VERSION}\\\""
fi

echo -e "${YELLOW}Building project with PlatformIO...${NC}"
pio run -e "$PIO_ENV"
ok "Build successful"
echo ""

[ -d "$BUILD_DIR" ] || die "Build directory not found: $BUILD_DIR"

BOOTLOADER="$BUILD_DIR/bootloader.bin"
PARTITIONS="$BUILD_DIR/partitions.bin"
FIRMWARE="$BUILD_DIR/firmware.bin"

echo "Checking required files..."
for f in "$BOOTLOADER" "$PARTITIONS" "$BOOT_APP0_PATH" "$FIRMWARE"; do
    if [ -z "$f" ] || [ ! -f "$f" ]; then
        die "Missing required binary: ${f:-boot_app0.bin}"
    fi
    ok "Found $(basename "$f")"
done

# ----------------------------------------------------------------- merge ----
# Address map for ESP32-S3:
#   0x0000  bootloader      (note: 0x1000 on the classic ESP32)
#   0x8000  partition table
#   0xe000  boot_app0 (OTA data)
#   0x10000 application firmware
mkdir -p "$DIST_DIR"

echo -e "\n${YELLOW}Merging firmware...${NC}"
"$ESPTOOL" --chip "$CHIP" "$CMD_MERGE" \
    -o "$OUTPUT_FILE" \
    --flash-mode "$FLASH_MODE" \
    --flash-freq "$FLASH_FREQ" \
    --flash-size "$FLASH_SIZE" \
    0x0     "$BOOTLOADER" \
    0x8000  "$PARTITIONS" \
    0xe000  "$BOOT_APP0_PATH" \
    0x10000 "$FIRMWARE"

# ---------------------------------------------------------------- report ----
OUTPUT_BASE=$(basename "$OUTPUT_FILE")
FILE_SIZE=$(du -h "$OUTPUT_FILE" | cut -f1)

# Checksum so a released binary can be verified later
if command -v shasum >/dev/null 2>&1; then
    (cd "$DIST_DIR" && shasum -a 256 "$OUTPUT_BASE" > "${OUTPUT_BASE}.sha256")
elif command -v sha256sum >/dev/null 2>&1; then
    (cd "$DIST_DIR" && sha256sum "$OUTPUT_BASE" > "${OUTPUT_BASE}.sha256")
fi

# Stable path for the flash script / CI that just wants "the newest build"
cp -f "$OUTPUT_FILE" "${DIST_DIR}/${PROJECT_NAME}_latest.bin"

echo -e "\n${GREEN}✓ Successfully merged firmware!${NC}"
echo -e "Output file: ${GREEN}${OUTPUT_FILE}${NC} (${YELLOW}${FILE_SIZE}${NC})"
echo -e "Also copied to: ${DIST_DIR}/${PROJECT_NAME}_latest.bin"

echo -e "\n${YELLOW}To flash it:${NC}"
echo "  ./flash_firmware.sh                      # newest build, auto-detect port"
echo "  ./flash_firmware.sh \"$OUTPUT_FILE\"   # this exact build"