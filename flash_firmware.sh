#!/usr/bin/env bash
# Flash Merged Firmware Script
#
# Usage:
#   ./flash_firmware.sh                                  # newest build + auto-detected port
#   ./flash_firmware.sh [FIRMWARE_FILE] [PORT] [BAUD]    # explicit
#   ./flash_firmware.sh -e                               # erase whole flash first
#
# Examples:
#   ./flash_firmware.sh dist/MatrixClock_v1.0.0.bin /dev/cu.usbmodem2101
#   ./flash_firmware.sh dist/MatrixClock_v1.0.0.bin COM3 460800

set -Eeuo pipefail

# ---------------------------------------------------------------- config ----
PROJECT_NAME="MatrixClock"
CHIP="esp32s3"
DIST_DIR="${DIST_DIR:-dist}"
DEFAULT_BAUD="921600"
ERASE_FIRST=0

if [ -t 1 ]; then
    RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; NC='\033[0m'
else
    RED=''; GREEN=''; YELLOW=''; NC=''
fi

die()  { echo -e "${RED}Error: $*${NC}" >&2; exit 1; }
warn() { echo -e "${YELLOW}Warning: $*${NC}" >&2; }

usage() {
    cat <<EOF
Usage:
  ./flash_firmware.sh [-e] [FIRMWARE_FILE] [PORT] [BAUD_RATE]

Options:
  -e, --erase   Erase the entire flash before writing (clears NVS / WiFi config)
  -h, --help    Show this help

With no arguments the newest *.bin under ./${DIST_DIR} (or the current
directory) is used, and the serial port is auto-detected. If exactly one
port is found it is used directly; if several are found, gum is used to
pick one interactively.

Common ports:
  macOS:   /dev/cu.usbmodem*, /dev/cu.usbserial-*
  Linux:   /dev/ttyACM0, /dev/ttyUSB0
  Windows: COM3, COM4, ...
EOF
}

# ------------------------------------------------------------------ args ----
# Plain variables instead of arrays: macOS still ships bash 3.2, where an
# empty array expansion under `set -u` aborts the script.
FIRMWARE_FILE=""
PORT=""
BAUD=""
ARG_COUNT=0

while [ $# -gt 0 ]; do
    case "$1" in
        -e|--erase) ERASE_FIRST=1 ;;
        -h|--help)  usage; exit 0 ;;
        -*)         die "Unknown option: $1" ;;
        *)
            ARG_COUNT=$((ARG_COUNT + 1))
            case "$ARG_COUNT" in
                1) FIRMWARE_FILE="$1" ;;
                2) PORT="$1" ;;
                3) BAUD="$1" ;;
                *) die "Too many arguments: $1" ;;
            esac
            ;;
    esac
    shift
done

BAUD="${BAUD:-$DEFAULT_BAUD}"

echo -e "${GREEN}=== ESP32-S3 Flash Tool ===${NC}\n"

# -------------------------------------------------------------- firmware ----
# Prefer the newest binary produced by build_firmware.sh
find_latest_firmware() {
    local newest="" f
    for f in "${DIST_DIR}"/*.bin ./*.bin; do
        [ -f "$f" ] || continue
        case "$(basename "$f")" in
            "${PROJECT_NAME}_latest.bin") continue ;;   # a duplicate of a versioned file
        esac
        if [ -z "$newest" ] || [ "$f" -nt "$newest" ]; then
            newest="$f"
        fi
    done
    printf '%s' "$newest"
}

if [ -z "$FIRMWARE_FILE" ]; then
    FIRMWARE_FILE=$(find_latest_firmware)
    if [ -z "$FIRMWARE_FILE" ]; then
        echo -e "${RED}No .bin file found in ./${DIST_DIR} or the current directory.${NC}\n"
        usage
        exit 1
    fi
    echo -e "${YELLOW}Auto-selected firmware:${NC} $FIRMWARE_FILE"
fi

[ -f "$FIRMWARE_FILE" ] || die "Firmware file not found: $FIRMWARE_FILE"

# Verify the checksum written by build_firmware.sh, when present
SHA_FILE="${FIRMWARE_FILE}.sha256"
if [ -f "$SHA_FILE" ]; then
    SHA_DIR=$(dirname "$FIRMWARE_FILE")
    if command -v shasum >/dev/null 2>&1; then
        (cd "$SHA_DIR" && shasum -a 256 -c "$(basename "$SHA_FILE")" >/dev/null 2>&1) \
            && echo -e "${GREEN}✓${NC} Checksum verified" || warn "Checksum mismatch for $FIRMWARE_FILE"
    elif command -v sha256sum >/dev/null 2>&1; then
        (cd "$SHA_DIR" && sha256sum -c "$(basename "$SHA_FILE")" >/dev/null 2>&1) \
            && echo -e "${GREEN}✓${NC} Checksum verified" || warn "Checksum mismatch for $FIRMWARE_FILE"
    fi
fi

# ------------------------------------------------------------------ port ----
detect_ports() {
    local p
    for p in /dev/cu.usbmodem* /dev/cu.usbserial-* /dev/cu.wchusbserial* \
             /dev/cu.SLAB_USBtoUART* /dev/ttyACM[0-9]* /dev/ttyUSB[0-9]*; do
        [ -e "$p" ] && printf '%s\n' "$p"
    done
}

# Ask the user to pick a port with gum, which is nicer than printing a list
# and asking them to re-run the command.
choose_port_with_gum() {
    local ports="$1" picked
    if ! command -v gum >/dev/null 2>&1; then
        echo -e "${RED}Multiple serial ports detected:${NC}" >&2
        printf '%s\n' "$ports" | sed 's/^/  /' >&2
        echo "" >&2
        echo -e "${YELLOW}Install gum to pick one interactively:${NC}" >&2
        echo "  macOS:  brew install gum" >&2
        echo "  Linux:  sudo apt install gum   (or see https://github.com/charmbracelet/gum)" >&2
        echo "" >&2
        echo -e "${YELLOW}Or pass the port explicitly:${NC}" >&2
        echo "  ./flash_firmware.sh \"$FIRMWARE_FILE\" /dev/cu.usbmodemXXXX" >&2
        exit 1
    fi
    if [ ! -t 0 ]; then
        die "Multiple serial ports detected but stdin is not a terminal. Pass the port explicitly."
    fi
    picked=$(printf '%s\n' "$ports" | gum choose --header "Select the ESP32-S3 serial port:") || true
    [ -n "$picked" ] || die "No port selected"
    printf '%s' "$picked"
}

if [ -z "$PORT" ]; then
    # mapfile is bash 4+, so keep the result as a newline separated string
    FOUND=$(detect_ports || true)
    FOUND_COUNT=$(printf '%s' "$FOUND" | grep -c . || true)
    case "${FOUND_COUNT:-0}" in
        0) echo -e "${RED}No serial port detected. Plug the board in or pass the port explicitly.${NC}\n"
           usage; exit 1 ;;
        1) PORT=$(printf '%s' "$FOUND" | head -n1)
           echo -e "${YELLOW}Auto-detected port:${NC} $PORT" ;;
        *) PORT=$(choose_port_with_gum "$FOUND")
           echo -e "${YELLOW}Selected port:${NC} $PORT" ;;
    esac
fi

# Windows COM ports are not filesystem entries, so only check the POSIX case
if [[ ! "$PORT" =~ ^COM[0-9]+$ ]] && [ ! -e "$PORT" ]; then
    warn "Port $PORT does not exist. Attempting to flash anyway..."
fi

# --------------------------------------------------------------- esptool ----
# esptool v4 uses snake_case subcommands/flags, v5 uses kebab-case.
ESPTOOL=""
for candidate in esptool esptool.py; do
    if command -v "$candidate" >/dev/null 2>&1; then ESPTOOL="$candidate"; break; fi
done
[ -n "$ESPTOOL" ] || die "esptool not found. Install it with: pip install esptool"

ESPTOOL_MAJOR=$("$ESPTOOL" version 2>/dev/null | grep -oE '[0-9]+' | head -n1 || true)
ESPTOOL_MAJOR="${ESPTOOL_MAJOR:-4}"
if [ "$ESPTOOL_MAJOR" -ge 5 ]; then
    CMD_WRITE="write-flash"; CMD_ERASE="erase-flash"
    OPT_BEFORE="default-reset"; OPT_AFTER="hard-reset"
else
    CMD_WRITE="write_flash"; CMD_ERASE="erase_flash"
    OPT_BEFORE="default_reset"; OPT_AFTER="hard_reset"
fi

# ----------------------------------------------------------------- flash ----
echo ""
echo "Firmware:  $FIRMWARE_FILE ($(du -h "$FIRMWARE_FILE" | cut -f1))"
echo "Port:      $PORT"
echo "Baud rate: $BAUD"
echo "esptool:   $ESPTOOL (major $ESPTOOL_MAJOR)"
echo ""

if [ "$ERASE_FIRST" -eq 1 ]; then
    warn "Erasing the entire flash - saved WiFi credentials and NVS settings will be lost."
    "$ESPTOOL" --chip "$CHIP" --port "$PORT" --baud "$BAUD" "$CMD_ERASE"
fi

echo -e "${YELLOW}Flashing firmware...${NC}"
"$ESPTOOL" --chip "$CHIP" \
    --port "$PORT" \
    --baud "$BAUD" \
    --before "$OPT_BEFORE" \
    --after "$OPT_AFTER" \
    "$CMD_WRITE" -z \
    --flash-mode dio \
    --flash-freq 80m \
    --flash-size detect \
    0x0 "$FIRMWARE_FILE"

# set -e already aborts on failure, so reaching this point means success
echo -e "\n${GREEN}✓ Successfully flashed firmware!${NC}"
echo -e "${GREEN}Device should now reset and boot${NC}"
echo -e "\n${YELLOW}Serial monitor:${NC} pio device monitor -p $PORT -b 115200"