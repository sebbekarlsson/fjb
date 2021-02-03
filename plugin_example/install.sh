cd ..
make clean; make; make install
cd plugin_example
make clean; make ; wait
cp ./libmyplugin.so ../examples/jsx/.plugins/.
