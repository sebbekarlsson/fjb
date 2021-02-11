P=$(pwd)

sudo apt install gcc wget --yes
wget https://nodejs.org/dist/v14.15.5/node-v14.15.5-linux-x64.tar.xz
tar xf node-v14.15.5-linux-x64.tar.xz
export PATH=$PATH:$P/node-v14.15.5-linux-x64/bin

npm install -g yarn

./build.sh

cd $P

mkdir -p $HOME/.local/lib && mkdir -p $HOME/.local/bin && mkdir -p $HOME/.local/include
export PATH=$PATH:$HOME/.local/bin


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

cd src/test_projects/with_lodash/
yarn install

cd $P
cd tests

ls -lart

../pull_deps.sh

cp ./gpp.out $HOME/.local/bin/gpp
sudo cp ./gpp.out /usr/bin/gpp

make && ./test.out
