#!/bin/bash

# Define color palette
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[0;37m'
NC='\033[0m' # No color

# Check if progress bar should be enabled
PROGRESS_BAR=false

# Parse options
while getopts "p" opt; do
  case $opt in
    p) PROGRESS_BAR=true ;;
    \?) echo "Usage: $0 [-p]"; exit 1 ;;
  esac
done

# Find the server's PID by looking for the process name "42server"
SERVER_PID=$(pgrep -f 42server)

# Check if server is running
if [ -z "$SERVER_PID" ]; then
    echo -e "${RED}Error: 42server is not running!${NC}"
    exit 1
fi

# Directory containing the test files
DIR="testFiles"

# List of test files
FILES=("small.txt" "medium.txt" "large.txt" "huge.txt")

# Function to format the file size in human-readable format
human_readable_size() {
    local SIZE=$1
    if (( SIZE < 1024 )); then
        echo "${SIZE} B"
    elif (( SIZE < 1048576 )); then
        echo "$(echo "scale=2; $SIZE/1024" | bc) KB"
    elif (( SIZE < 1073741824 )); then
        echo "$(echo "scale=2; $SIZE/1048576" | bc) MB"
    else
        echo "$(echo "scale=2; $SIZE/1073741824" | bc) GB"
    fi
}

# Function to get the file size in bytes
get_file_size() {
    if command -v stat &> /dev/null; then
        # Use stat if available (Linux)
        FILE_SIZE=$(stat -f%z "$1")
    elif command -v gstat &> /dev/null; then
        # Use gstat on macOS (Homebrew version of stat)
        FILE_SIZE=$(gstat -c %s "$1")
    else
        # Use wc as a fallback
        FILE_SIZE=$(wc -c < "$1")
    fi
    echo $FILE_SIZE
}

# Simple progress bar function
progress_bar() {
    local current=$1
    local total=$2
    local width=50  # Width of the progress bar (in characters)
    local percent=$(( 100 * current / total ))
    local progress=$(( width * current / total ))
    local done=$(printf "%${progress}s" "" | tr " " "#")
    local remaining=$(printf "%$((width - progress))s" "" | tr " " " ")
    printf "\r[%-${width}s] %d%%" "$done$remaining" "$percent"
}

# Loop through each file
for FILE in "${FILES[@]}"; do
    echo -e "${CYAN}Running client for file: $FILE${NC}"
    
    # Check if the file exists
    if [[ ! -f "$DIR/$FILE" ]]; then
        echo -e "${RED}File $DIR/$FILE does not exist!${NC}"
        continue
    fi
    
    # Get the file size
    FILE_SIZE=$(get_file_size "$DIR/$FILE")
    
    # Display the file size in human-readable format
    echo -n -e "${YELLOW}File size of $FILE: ${NC}"
    human_readable_size $FILE_SIZE

    # Only display progress bar if enabled
    if [ "$PROGRESS_BAR" = true ]; then
        echo -n -e "${GREEN}Progress: ${NC}"
    fi
    
    # Capture the file content while displaying progress
    MESSAGE=""
    current=0
    progress_interval=500  # Update the progress bar every 500 characters
    while IFS= read -r -n1 char; do
        MESSAGE+="$char"
        current=$((current + 1))

        # Only update the progress bar every 'progress_interval' characters
        if [ "$PROGRESS_BAR" = true ] && (( current % progress_interval == 0 )); then
            progress_bar $current $FILE_SIZE
        fi
    done < "$DIR/$FILE"

    echo ""  # Move to the next line after the progress bar

    # Run the client with the file content
    echo -e "${PURPLE}Time taken for $FILE: ${NC}"
    time ./client $SERVER_PID "$MESSAGE"
    
    echo -e "${CYAN}-------------------------------------${NC}"
done

