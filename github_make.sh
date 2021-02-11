make GPP_PATH=./gpp.out
make install GPP_PATH=./gpp.out
sudo make install GPP_PATH=./gpp.out

echo "Building dependecy: libhashmap"
cd ./external/hashmap
make
cd ..

echo "Building dependecy: libjson"
cd ./external/libjson
make
cd ..

sudo cp *.a /usr/local/lib/.
sudo cp -r src/include /usr/local/include/fjb

cp gpp.out tests/.
