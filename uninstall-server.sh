#!/bin/bash

PROJECT_NAME=${1:-CppRest}

# Stop the server if running
./stop-server.sh "$PROJECT_NAME"

# Remove the binary link
if [ -L "/usr/local/bin/$PROJECT_NAME" ]; then
    echo "Removing symlink /usr/local/bin/$PROJECT_NAME..."
    sudo rm "/usr/local/bin/$PROJECT_NAME"
else
    echo "No symlink found for /usr/local/bin/$PROJECT_NAME"
fi

echo "$PROJECT_NAME uninstalled"
