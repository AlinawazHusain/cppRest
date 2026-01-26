#! /bin/bash

echo "Bilding project ...";


PROJECT_NAME=${1:-CppRest}
MAIN_CPP=${2:-src/main.cpp}
BUILD_TYPE=${3:-Debug} 



if [ -z "$PROJECT_NAME" ] || [ -z "$MAIN_CPP" ]; then
    echo "Usage: $0 <ProjectName> <MainCppFile>"
    exit 1
fi


if [[ ! "$BUILD_TYPE" =~ ^(Debug|Release|RelWithDebInfo|MinSizeRel)$ ]]; then
    echo "Invalid build type: $BUILD_TYPE"
    echo "Valid options: Debug, Release, RelWithDebInfo, MinSizeRel"
    exit 1

    
fi

echo "Building project..."
echo "Project name: $PROJECT_NAME"
echo "Main cpp: $MAIN_CPP"
echo "Build type: $BUILD_TYPE"

# Clean old build
rm -rf build
mkdir build
mkdir -p bin

# Generate CMake cache with variables
cmake -S . -B build \
      -DPROJECT_NAME=$PROJECT_NAME \
      -DMAIN_CPP=$MAIN_CPP\
      -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Build
cmake --build build --config $BUILD_TYPE

echo "Executable should be in bin/"