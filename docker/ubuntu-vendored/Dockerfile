FROM ubuntu:bionic

# Update repositories
RUN apt-get update

# Install misc
RUN apt-get install -y wget git make curl clang bison pkg-config

# Install compilation tools
RUN curl https://sh.rustup.rs -sSf | bash -s -- -y --profile minimal

RUN echo 'source $HOME/.cargo/env' >> /etc/profile

#RUN rm -f /usr/bin/python && ln -s /usr/bin/python3 /usr/bin/python

# Kerberos libs will be built there
ENV KRB5_HOME="/node-krb5/vendor"

# Install Node.js via n
ENV NPM_CONFIG_LOGLEVEL info
RUN git clone https://github.com/tj/n /n
RUN cd /n && make install
RUN n 14
RUN n 16

RUN mkdir -p /node-krb5
WORKDIR /node-krb5

COPY ./install_krb5.sh .
RUN ./install_krb5.sh

COPY . .

# Context is node-krb5 folder
COPY docker/ubuntu-vendored/run.sh /run.sh
ENTRYPOINT ["/run.sh"]
