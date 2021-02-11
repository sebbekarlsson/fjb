sudo apt-get update -y
sudo apt-get install --yes build-essential
sudo apt-get install --yes cmake
sudo apt-get install --yes gcc
sudo apt-get install --yes g++

./pull_deps.sh
sudo cp external/hashmap/*.a /usr/local/lib/.
sudo cp -r external/hashmap/src/include /usr/local/include/hashmap

sudo cp external/libjson/*.a /usr/local/lib/.
sudo cp -r external/libjson/src/include /usr/local/include/libjson
