echo "Downloading MIT Kerberos..."
wget https://kerberos.org/dist/krb5/1.16/krb5-1.16.1.tar.gz
echo "Extracting..."
mkdir deps
tar -xzf krb5-1.16.1.tar.gz --directory deps
cd deps/krb5-1.16.1/src
echo "Compiling"
./configure
make
echo "Installing requires root privilege"
sudo make install
echo "Cleaning files..."
cd ../../..
rm -rf deps
rm -rf krb5-1.16.1.tar.gz
