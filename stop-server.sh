#!/bin/bash

PROJECT_NAME=${1:-CppRest}
PID_FILE="./server.pid"

if [ ! -f "$PID_FILE" ]; then
    echo "$PROJECT_NAME is not running (no PID file found)"
    exit 1
fi

PID=$(cat "$PID_FILE")

# Check if process exists
if kill -0 "$PID" 2>/dev/null; then
    echo "Stopping $PROJECT_NAME (PID $PID)..."
    kill "$PID"
    rm -f "$PID_FILE"
    echo "$PROJECT_NAME stopped"
else
    echo "Process not running, removing stale PID file"
    rm -f "$PID_FILE"
fi
