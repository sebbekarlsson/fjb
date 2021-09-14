cd ..
./pull_deps.sh
./build.sh
cd tests
cd ./fjb-samples
./init.sh
cd ..
mkdir -p build
cd build && cmake .. && make && ./test
# make clean
# make
# ./test.out
# ./setup_chromedriver.sh
# ./web.sh
