#!/bin/bash

PROJECT_NAME=${1:-CppRest}
PID_FILE="./server.pid"
EXEC="/usr/local/bin/$PROJECT_NAME"

LOG_DIR=${2:-"./logs"}  
mkdir -p "$LOG_DIR"  
LOG_FILE="$LOG_DIR/server.log"

# Check if executable exists in /usr/local/bin
if [ ! -x "$EXEC" ]; then
    echo "Error: $PROJECT_NAME not installed , use install-server.sh before starting server."
    exit 1
fi


# Check if server is already running
if [ -f "$PID_FILE" ]; then
    PID=$(cat "$PID_FILE")
    if kill -0 "$PID" 2>/dev/null; then
        echo "$PROJECT_NAME is already running (PID $PID)"
        exit 1
    else
        echo "Removing stale PID file"
        rm -f "$PID_FILE"
    fi
fi



# Start the server in background
echo "Starting $PROJECT_NAME..."

nohup "$EXEC" > "$LOG_FILE" 2>&1 &

# Save its PID
echo $! > "$PID_FILE"
echo "$PROJECT_NAME started with PID $(cat $PID_FILE)"




echo "Server output is being logged to server.log"