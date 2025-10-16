#!/bin/bash
# ESP32-S3 Firmware Build Script
# This script merges bootloader, partitions, boot_app0, and firmware into a single binary
# Execute this script in the project root directory
set -e  # Exit on error

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
PROJECT_NAME="MatrixClock"

# Get latest git tag as version
if git rev-parse --git-dir > /dev/null 2>&1; then
    VERSION=$(git describe --tags --abbrev=0 2>/dev/null || echo "1.0")
    # Remove 'v' prefix if exists
    VERSION=${VERSION#v}
else
    echo -e "${YELLOW}Warning: Not a git repository, using default version${NC}"
    VERSION="1.0"
fi

BUILD_DIR=".pio/build/esp32-s3-devkitm-1"
BOOT_APP0_PATH="$HOME/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin"
OUTPUT_FILE="${PROJECT_NAME}_v${VERSION}.bin"
FLASH_SIZE="16MB"  # ESP32-S3-N16R8 has 16MB Flash

echo -e "${GREEN}=== ESP32-S3 Build Firmware Tool ===${NC}\n"
echo "Project: $PROJECT_NAME"
echo "Version: $VERSION"
echo "Flash Size: $FLASH_SIZE"
echo "Output: $OUTPUT_FILE"
echo ""

# Check if pio command is available
if ! command -v pio &> /dev/null; then
    echo -e "${RED}Error: pio command not found${NC}"
    echo "Please install PlatformIO Core"
    exit 1
fi

# Build the project
echo -e "${YELLOW}Building project with PlatformIO...${NC}"
pio run

if [ $? -ne 0 ]; then
    echo -e "${RED}Error: Build failed${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Build successful${NC}\n"

# Check if build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo -e "${RED}Error: Build directory not found: $BUILD_DIR${NC}"
    exit 1
fi

# Check required files
BOOTLOADER="$BUILD_DIR/bootloader.bin"
PARTITIONS="$BUILD_DIR/partitions.bin"
FIRMWARE="$BUILD_DIR/firmware.bin"

echo "Checking required files..."

if [ ! -f "$BOOTLOADER" ]; then
    echo -e "${RED}Error: bootloader.bin not found${NC}"
    exit 1
fi
echo -e "${GREEN}✓${NC} Found bootloader.bin"

if [ ! -f "$PARTITIONS" ]; then
    echo -e "${RED}Error: partitions.bin not found${NC}"
    exit 1
fi
echo -e "${GREEN}✓${NC} Found partitions.bin"

if [ ! -f "$BOOT_APP0_PATH" ]; then
    echo -e "${RED}Error: boot_app0.bin not found at $BOOT_APP0_PATH${NC}"
    exit 1
fi
echo -e "${GREEN}✓${NC} Found boot_app0.bin"

if [ ! -f "$FIRMWARE" ]; then
    echo -e "${RED}Error: firmware.bin not found${NC}"
    exit 1
fi
echo -e "${GREEN}✓${NC} Found firmware.bin"

# Check if esptool is available
if ! command -v esptool &> /dev/null; then
    echo -e "${RED}Error: esptool not found${NC}"
    echo "Please install esptool: pip install esptool"
    exit 1
fi
echo -e "${GREEN}✓${NC} Found esptool"

echo -e "\n${YELLOW}Merging firmware...${NC}"

# Merge all binaries into one
# Address mapping for ESP32-S3:
# 0x0000  - bootloader
# 0x8000  - partition table
# 0xe000  - boot_app0 (OTA data)
# 0x10000 - application firmware
esptool --chip esp32s3 merge-bin \
    -o "$OUTPUT_FILE" \
    --flash-mode dio \
    --flash-freq 80m \
    --flash-size "$FLASH_SIZE" \
    0x0 "$BOOTLOADER" \
    0x8000 "$PARTITIONS" \
    0xe000 "$BOOT_APP0_PATH" \
    0x10000 "$FIRMWARE"

if [ $? -eq 0 ]; then
    echo -e "\n${GREEN}✓ Successfully merged firmware!${NC}"
    echo -e "${GREEN}Output file: $OUTPUT_FILE${NC}"
    
    # Show file size
    FILE_SIZE=$(du -h "$OUTPUT_FILE" | cut -f1)
    echo -e "File size: ${YELLOW}$FILE_SIZE${NC}"
    
    echo -e "\n${YELLOW}To flash this merged firmware:${NC}"
    echo "esptool --chip esp32s3 --port /dev/cu.usbmodem --baud 921600 write_flash 0x0 $OUTPUT_FILE"
    
    echo -e "\n${YELLOW}Or use the flash script:${NC}"
    echo "./flash_firmware.sh MatrixClock_v1.0.0.bin /dev/cu.usbmodem"
else
    echo -e "${RED}Error: Failed to merge firmware${NC}"
    exit 1
fi
