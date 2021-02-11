P=$(pwd)
make GPP_PATH=./gpp.out
make install GPP_PATH=./gpp.out
sudo make install GPP_PATH=./gpp.out
cd $P

echo "Building dependecy: libhashmap"
cd ./external/hashmap
make
cd $P

echo "Building dependecy: libjson"
cd ./external/libjson
make
cd $P

sudo cp *.a /usr/local/lib/.
sudo cp -r src/include /usr/local/include/fjb

cp gpp.out tests/.

cd $P
./build.sh
cd $P
