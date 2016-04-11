# some usefule module.

echo "install lua 5.3.2"
rm -rf tmp
mkdir -p tmp
cd tmp
curl -R -O http://www.lua.org/ftp/lua-5.3.2.tar.gz
tar zxf lua-5.3.2.tar.gz
cd lua-5.3.2
sudo make macosx && sudo make install
cd -
rm -rf tmp

echo "install lua-coverage requires sudo"
sudo luarocks install luacov

