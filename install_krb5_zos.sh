mkdir deps
echo "Downloading and extracting MIT Kerberos library for zos..."
cd deps
curl https://codeload.github.com/ibmruntimes/libkrb5-zos/tar.gz/v1.16.3-zos  | iconv -t 819 -f 1047 | gzip -d  | tar -xfUXo -
export KRB5_HOME=$PWD/libkrb5-zos-1.16.3-zos
echo "Tagging files..."
chtag -tc 819 -R $KRB5_HOME
chtag -b -R $KRB5_HOME/lib
echo "Cleaning files..."
rm -rf v1.16.3-zos.tar.gz
cd ..
