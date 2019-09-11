mkdir deps
echo "Downloading and extracting MIT Kerberos library for zos..."
cd deps
_ENCODE_FILE_NEW=BINARY curl https://codeload.github.com/ibmruntimes/libkrb5-zos/tar.gz/v1.16.3-zos --output v1.16.3-zos.tar.gz
gzip -d v1.16.3-zos.tar.gz
tar -xfUXo v1.16.3-zos.tar
export KRB5_HOME=$PWD/libkrb5-zos-1.16.3-zos
echo "Tagging files..."
chtag -tc 819 -R $KRB5_HOME
chtag -b -R $KRB5_HOME/lib
echo "Cleaning files..."
rm -rf v1.16.3-zos.tar
cd ..
