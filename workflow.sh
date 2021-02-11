sudo apt install gcc --yes

P=$(pwd)

./build.sh

cd $P

mkdir -p $HOME/.local/lib && mkdir -p $HOME/.local/bin && mkdir -p $HOME/.local/include

cd ./external/hashmap
make
make install
cd $P

cd ./external/libjson
make
make install
cd $P

make install

cd $P

ls -lart

cd tests

ls -lart

../pull_deps.sh

make && ./test.out
