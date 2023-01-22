FROM archlinux/archlinux:base

RUN pacman --noconfirm -Syu

# Install misc
RUN pacman --noconfirm -S wget git python3 make clang pkg-config

RUN curl https://sh.rustup.rs -sSf | bash -s -- -y --profile minimal

RUN echo 'source $HOME/.cargo/env' >> /etc/profile

# Install Kerberos libs
RUN pacman --noconfirm -S krb5

# Install Node.js via n
ENV NPM_CONFIG_LOGLEVEL info
RUN git clone https://github.com/tj/n /n
RUN cd /n && make install
RUN n 14
RUN n 16

RUN mkdir -p /node-krb5
WORKDIR /node-krb5

COPY . .

# Context is node-krb5 folder
COPY docker/archlinux/run.sh /run.sh
ENTRYPOINT ["/run.sh"]
