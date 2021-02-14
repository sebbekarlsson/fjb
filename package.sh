if $(test -f ./gpp.out); then
  GPP_PATH=./gpp.out;
elif $(test -f ../gpp.out); then
  GPP_PATH=../gpp.out;
else
  GPP_PATH=$(which gpp);
fi

echo "Using gpp:"
echo $GPP_PATH

VERSION=$(./version.sh)

PKGNAME=fjb_$VERSION

rm -rf $PKGNAME

echo "Creating $PKGNAME"

mkdir $PKGNAME


mkdir $PKGNAME/usr
mkdir $PKGNAME/usr/local
mkdir $PKGNAME/usr/local/bin
mkdir $PKGNAME/usr/local/lib
mkdir $PKGNAME/usr/local/include
cp ./fjb.out $PKGNAME/usr/local/bin/fjb

cp ./libjson.a $PKGNAME/usr/local/lib/.
cp ./libhashmap.a $PKGNAME/usr/local/lib/.
cp ./libscss.a $PKGNAME/usr/local/lib/.

cp -r src/include $PKGNAME/usr/local/include/fjb

mkdir $PKGNAME/DEBIAN

$GPP_PATH ./pkg.gpp > $PKGNAME/DEBIAN/control

echo "Building..."
dpkg-deb --build $PKGNAME
echo "Done"
