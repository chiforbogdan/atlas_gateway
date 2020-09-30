#!/bin/bash
echo "THIS SCRIPT WILL NOW BEGIN INSTALLING ALL THE DEPENDENCIES NEEDED BY THE ATLAS_GATEWAY"

# Update repository definitions
echo "*********************************************************"
echo "*********** Updating repository definitions *************"
echo "*********************************************************"
sudo apt-get update
echo "*********************************************************"
echo "************ Repository definitions updated *************"
echo "*********************************************************"

# Install Eclipse Paho and its dependencies, with support for C++
echo "*********************************************************"
echo "***** Installing Eclipse Paho and its dependencies ******"
echo "*********************************************************"
# __dependencies__
echo "************ Step 1. Installing dependencies ************"
sudo apt-get install -y build-essential gcc make cmake cmake-gui cmake-curses-gui
sudo apt-get install -y libssl-dev
sudo apt-get install -y git
sudo apt-get install -y libjsoncpp-dev
echo "******************* Step 1 finished! ********************"
# __build Paho with support for C__
echo "*** Step 2. Building Eclipse Paho with support for C ****"
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
git checkout v1.3.1
cmake -Bbuild -H. -DPAHO_WITH_SSL=ON -DPAHO_ENABLE_TESTING=OFF
sudo cmake --build build/ --target install
sudo ldconfig
cd ..
echo "******************* Step 2 finished! ********************"
# __build Paho with support for C++__
echo "** Step 3. Building Eclipse Paho with support for C++ ***"
git clone https://github.com/eclipse/paho.mqtt.cpp
cd paho.mqtt.cpp
cmake -Bbuild -H. -DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=FALSE -DPAHO_WITH_SSL=TRUE -DPAHO_BUILD_TESTS=FALSE
sudo cmake --build build/ --target install
sudo ldconfig
cd ..
echo "******************* Step 3 finished! ********************"
echo "*********************************************************"
echo "********* Eclipse Paho successfully installed! **********"
echo "*********************************************************"

# Build and install libcoap
echo "*********************************************************"
echo "***************** Adding suport for CoAP ****************"
echo "*********************************************************"
sudo apt-get install -y autotools-dev autoconf automake m4 libtool pkg-config
git clone https://github.com/obgm/libcoap.git
cd libcoap
git checkout develop
sudo sh ./autogen.sh
#cp ../ltmain.sh .
sudo sh ./autogen.sh
sudo sh ./configure --with-openssl --enable-shared --disable-documentation --disable-examples
make
sudo make install
cd ..
echo "*********************************************************"
echo "****************** CoAP support added *******************"
echo "*********************************************************"

# Install BOOST libraries
echo "*********************************************************"
echo "*************** Addind support for BOOST ****************"
echo "*********************************************************"
#sudo apt-get install -y libboost-system-dev libboost-log-dev libboost-program-options-dev libboost-regex-dev
wget https://dl.bintray.com/boostorg/release/1.69.0/source/boost_1_69_0.tar.bz2
tar xf boost_1_69_0.tar.bz2
cd boost_1_69_0/
./bootstrap.sh --with-libraries=system,log,program_options,regex,filesystem
sudo ./b2 install --prefix=/usr
cd ..
rm boost_1_69_0.tar.bz2
echo "*********************************************************"
echo "***************** BOOST support added *******************"
echo "*********************************************************"

# Install Sqlite3
echo "*********************************************************"
echo "*************** Adding support for Sqlite ***************"
echo "*********************************************************"
sudo apt-get install -y sqlite3 libsqlite3-dev
echo "*********************************************************"
echo "***************** Sqlite support added ******************"
echo "*********************************************************"

# Add support for UUID
echo "*********************************************************"
echo "**************** Adding support for UUID ****************"
echo "*********************************************************"
sudo apt-get install -y uuid-dev uuid uuid-runtime
echo "*********************************************************"
echo "****************** UUID support added *******************"
echo "*********************************************************"

# Add support for JSON
echo "*********************************************************"
echo "**************** Adding support for JSON ****************"
echo "*********************************************************"
sudo apt-get install -y libjsoncpp-dev libjsoncpp1
echo "*********************************************************"
echo "****************** JSON support added *******************"
echo "*********************************************************"

# Add support for nghttp2
echo "*********************************************************"
echo "**************** Adding support for nghttp2 *************"
echo "*********************************************************"
git clone https://github.com/nghttp2/nghttp2.git
cd nghttp2
git submodule update --init
autoreconf -i
automake
autoconf
./configure --enable-asio-lib
make
sudo make install
cd ..
echo "*********************************************************"
echo "****************** nghttp2 support added ****************"
echo "*********************************************************"

# Install and build customized Eclipse Mosquitto
echo "*********************************************************"
echo "************* Installing Eclipse Mosquitto **************"
echo "*********************************************************"
#sudo apt-get install -y snap
#sudo snap install mosquitto
git clone https://github.com/chiforbogdan/mosquitto.git
cd mosquitto
git checkout atlas_plugin
make
sudo make install
cd ..
echo "*********************************************************"
echo "******* Eclipse Mosquitto successfully installed! *******"
echo "*********************************************************"
echo "  ALL ATLAS FRAMEWORK DEPENDENCIES HAVE BEEN INSTALLED"
