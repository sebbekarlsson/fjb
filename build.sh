./pull_deps.sh

echo "Dependencies done."

echo "Building fjb..."
make

echo "Cleaning upp..."
rm -rf ./gpp

echo "---------------------****---------------------"
echo "---------------------DONE---------------------"
echo "---------------------****---------------------"
