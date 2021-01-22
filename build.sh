echo "Pulling dependencies..."
git submodule update --init --recursive
git clone https://github.com/sebbekarlsson/gpp.git --recurse-submodules

echo "Building dependency: gpp"
cd gpp && make
cd ../
mv gpp/a.out ./gpp.out

echo "Dependencies done."

echo "Building fjb..."
make

echo "Cleaning upp..."
rm -rf ./gpp

echo "---------------------****---------------------"
echo "---------------------DONE---------------------"
echo "---------------------****---------------------"
