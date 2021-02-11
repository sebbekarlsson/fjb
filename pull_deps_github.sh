sudo apt-get update -y
sudo apt-get install --yes build-essential
sudo apt-get install --yes gcc
sudo apt-get install --yes g++

./pull_deps.sh

sudo mkdir -p /usr/local
sudo mkdir -p /usr/local/lib
sudo mkdir -p /usr/local/include
sudo mkdir -p /usr/local/bin

sudo cp external/hashmap/*.a /usr/local/lib
sudo cp -r external/hashmap/src/include /usr/local/include/hashmap

sudo cp external/libjson/*.a /usr/local/lib
sudo cp -r external/libjson/src/include /usr/local/include/libjson

cp -r external/hashmap/*.a tests/.
cp -r external/libjson/*.a tests/.
