OLD=$(pwd)
BENCHPATH=$OLD/benchmark
cd benchmark
rm -rf ./dist*
# yarn add esbuild
virtualenv -p /usr/bin/python3.9 ./venv
$BENCHPATH/venv/bin/python setup.py develop
cd $OLD
$BENCHPATH/venv/bin/python $BENCHPATH/generate.py $1 $2 $3 $4
