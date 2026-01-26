#!/bin/bash


PROJECT_NAME=${1:-CppRest}


if [ ! -f "bin/${PROJECT_NAME}" ]; then
    echo "Error: bin/${PROJECT_NAME} does not exist!"
    exit 1
fi


sudo ln -sf "$(pwd)/bin/${PROJECT_NAME}" /usr/local/bin/${PROJECT_NAME}

echo "Linked bin/${PROJECT_NAME} to /usr/local/bin/${PROJECT_NAME}"
