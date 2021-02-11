echo "Pulling dependencies..."
git submodule update --init --recursive
git clone https://github.com/sebbekarlsson/gpp.git --recurse-submodules

echo "Building dependency: gpp"
cd gpp && make
cd ../
mv gpp/a.out ./gpp.out

echo "Building dependecy: libhashmap"
cd ./external/hashmap
make

echo "Building dependecy: libjson"
cd ./external/libjson
make
