#! /bin/bash


PROJECT_NAME=${1:-CppRest}

LOG_DIR=${2:-"./logs"}  
mkdir -p "$LOG_DIR"  
LOG_FILE="$LOG_DIR/server.log"


./bin/$PROJECT_NAME 2>&1 | tee -a "$LOG_FILE"
