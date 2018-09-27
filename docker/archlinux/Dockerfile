FROM base/archlinux

# Update repositories
RUN echo "Server = http://mirror.aarnet.edu.au/pub/archlinux/$repo/os/$arch" >> /etc/pacman.d/mirrorlist
RUN pacman --noconfirm -Syu

# Install Node.js
ENV NPM_CONFIG_LOGLEVEL info
RUN pacman --noconfirm -S nodejs npm

# Install misc
RUN pacman --noconfirm -S wget git vim sudo

# Install compilation tools
RUN pacman --noconfirm -S python2 gcc bison make

RUN mkdir -p /node-krb5
ADD ./run.sh /run.sh
ENTRYPOINT ["/run.sh"]
