sudo apt install gcc --yes

P=$(pwd)

./build.sh

cd $P

mkdir -p $HOME/.local/lib && mkdir -p $HOME/.local/bin && mkdir -p $HOME/.local/include

make install

cd $P

ls -lart

cd tests

ls -lart

make && ./test.out
