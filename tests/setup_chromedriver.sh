path=$(pwd)

rm -rf driver
mkdir driver
cd driver
wget https://chromedriver.storage.googleapis.com/88.0.4324.96/chromedriver_linux64.zip

unzip chromedriver_linux64.zip

cd $path
