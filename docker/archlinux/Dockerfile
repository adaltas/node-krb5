FROM archlinux/base

# Update repositories
RUN echo "Server = http://mirror.aarnet.edu.au/pub/archlinux/$repo/os/$arch" >> /etc/pacman.d/mirrorlist
RUN pacman --noconfirm -Syu

# Install misc
RUN pacman --noconfirm -S wget git vim sudo 

# Install compilation tools
RUN pacman --noconfirm -S python2 gcc make

# Install Kerberos libs
RUN pacman --noconfirm -S krb5

# Install Node.js via n
ENV NPM_CONFIG_LOGLEVEL info
RUN git clone https://github.com/tj/n /n
RUN cd /n && make install
RUN n 6.17.1
RUN n 8.16.1
RUN n 10.16.3
RUN n 12.10.0

RUN mkdir -p /node-krb5
COPY ./run.sh /run.sh
ENTRYPOINT ["/run.sh"]
