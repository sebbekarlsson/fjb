sudo apt update --yes
sudo apt upgrade --yes

P=$(pwd)

./build.sh

cd $P

mkdir -p $HOME/.local/lib && mkdir -p $HOME/.local/bin && mkdir -p $HOME/.local/include

make install

cd $P

cd tests
make && ./test.out
