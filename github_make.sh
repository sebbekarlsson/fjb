make GPP_PATH=./gpp.out
make install GPP_PATH=./gpp.out
sudo make install GPP_PATH=./gpp.out

sudo cp *.a /usr/local/lib/.
sudo cp -r src/include /usr/local/include/fjb

cp gpp.out tests/.
