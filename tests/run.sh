cd ..
make clean
make
cd tests
cd ./fjb-samples
./init.sh
cd ..
make clean
make
./test.out

./setup_chromedriver.sh
./web.sh
