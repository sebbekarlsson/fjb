echo "Pulling dependencies..."
git submodule init
git submodule update
git submodule sync
git submodule update --init --recursive
git clone https://github.com/sebbekarlsson/gpp.git --recurse-submodules

echo "Building dependency: gpp"
cd gpp && make
cd ../
cp gpp/a.out ./gpp.out
cp gpp/a.out ./tests/gpp.out

echo "Building dependecy: libhashmap"
cd ./external/hashmap
make
cd ..

echo "Building dependecy: libjson"
cd ./external/libjson
make
cd ..
