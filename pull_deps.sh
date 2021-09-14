P=$(pwd)
echo "Pulling dependencies..."
git submodule init
git submodule update
git submodule sync
git submodule update --init --recursive
git clone https://github.com/sebbekarlsson/gpp.git --recurse-submodules

echo "Building dependency: gpp"
cd gpp;
git submodule init; git submodule update; git submodule sync;
make
make install
cd $P
cp gpp/a.out ./gpp.out
pwd
cp gpp/a.out ./tests/gpp.out
cd $P
rm -rf ./gpp
