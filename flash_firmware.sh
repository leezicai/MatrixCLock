#!/bin/bash

# Flash Merged Firmware Script
# Usage: ./flash_merged.sh [FIRMWARE_FILE] [PORT] [BAUD_RATE]
# Example: ./flash_merged.sh MatrixClock_v1.0.bin /dev/cu.usbmodem2101 921600
# Example: ./flash_merged.sh MatrixClock_v1.0.bin /dev/ttyUSB0

set -e

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Parse arguments
MERGED_FILE="${1}"
PORT="${2}"
BAUD="${3:-921600}"

echo -e "${GREEN}=== ESP32-S3 Flash Tool ===${NC}\n"

# Show usage if no arguments provided
if [ -z "$MERGED_FILE" ] || [ -z "$PORT" ]; then
    echo -e "${YELLOW}Usage:${NC}"
    echo "  ./flash_merged.sh [FIRMWARE_FILE] [PORT] [BAUD_RATE]"
    echo ""
    echo -e "${YELLOW}Examples:${NC}"
    echo "  ./flash_merged.sh MatrixClock_v1.0.bin /dev/cu.usbmodem2101"
    echo "  ./flash_merged.sh MatrixClock_v1.0.bin /dev/cu.usbmodem2101 921600"
    echo "  ./flash_merged.sh firmware.bin COM3 460800"
    echo ""
    echo -e "${YELLOW}Available .bin files in current directory:${NC}"
    ls -lh *.bin 2>/dev/null || echo "  No .bin files found"
    echo ""
    echo -e "${YELLOW}Common ports:${NC}"
    echo "  Linux:   /dev/cu.usbmodem2101, /dev/ttyACM0"
    echo "  macOS:   /dev/cu.usbserial-*, /dev/cu.usbmodem*"
    echo "  Windows: COM3, COM4, COM5, ..."
    exit 1
fi

# Check if merged firmware exists
if [ ! -f "$MERGED_FILE" ]; then
    echo -e "${RED}Error: Firmware file not found: $MERGED_FILE${NC}"
    echo ""
    echo -e "${YELLOW}Available .bin files in current directory:${NC}"
    ls -lh *.bin 2>/dev/null || echo "  No .bin files found"
    exit 1
fi

# Check if port exists (skip for Windows COM ports)
if [[ ! "$PORT" =~ ^COM[0-9]+$ ]] && [ ! -e "$PORT" ]; then
    echo -e "${RED}Warning: Port $PORT does not exist${NC}"
    echo "Attempting to flash anyway..."
    echo ""
fi

# Check if esptool is available
if ! command -v esptool &> /dev/null; then
    echo -e "${RED}Error: esptool not found${NC}"
    echo "Please install esptool: pip install esptool"
    exit 1
fi

echo "Firmware: $MERGED_FILE ($(du -h "$MERGED_FILE" | cut -f1))"
echo "Port: $PORT"
echo "Baud rate: $BAUD"
echo ""

# Erase flash (optional, comment out if not needed)
# echo -e "${YELLOW}Erasing flash...${NC}"
# esptool --chip esp32s3 --port "$PORT" erase_flash

# Flash the merged firmware
echo -e "${YELLOW}Flashing firmware...${NC}"
esptool --chip esp32s3 \
    --port "$PORT" \
    --baud "$BAUD" \
    --before default-reset \
    --after hard-reset \
    write-flash -z \
    --flash-mode dio \
    --flash-freq 80m \
    --flash-size detect \
    0x0 "$MERGED_FILE"

if [ $? -eq 0 ]; then
    echo -e "\n${GREEN}✓ Successfully flashed firmware!${NC}"
    echo -e "${GREEN}Device should now reset and boot${NC}"
else
    echo -e "\n${RED}Error: Failed to flash firmware${NC}"
    exit 1
fi
