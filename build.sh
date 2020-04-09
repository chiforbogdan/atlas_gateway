#!/bin/bash
echo "THIS SCRIPT WILL NOW BUILD THE ATLAS_GATEWAY SECURITY COMPONENTS"

# Create Build directory
echo "*********** Step 1. Creating build directory ************"
mkdir build
echo "******************* Step 1 finished! ********************"

# Buildint project
echo "******* Step 2. Building ATLAS_GATEWAY components *******"
cd build/
cmake ..
make
rm -r CMakeFiles/
rm CMakeCache.txt Makefile cmake_install.cmake
echo "******************* Step 2 finished! ********************"

# Finishing ATLAS_GATEWAY configuration
echo "*** Step 3. Finishing configuration of ATLAS_GATEWAY ****"
CONFIG_FILE=../src/mosquitto_plugin/config/mosquitto-atlas.conf
echo "$(grep -v 'auth_plugin' $CONFIG_FILE)" >$CONFIG_FILE
echo "auth_plugin $(pwd)/libatlas_gateway_mosquitto_plugin.so" >>$CONFIG_FILE
cp $CONFIG_FILE .
echo "******************* Step 3 finished! ********************"

echo "  ALL ATLAS_GATEWAY COMPONENTS HAVE BEEN BUILT"
