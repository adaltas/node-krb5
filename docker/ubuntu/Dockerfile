FROM ubuntu:xenial

# Update repositories
RUN apt-get update

# Install misc
RUN apt-get install -y wget git vim sudo make

# Install compilation tools
RUN apt-get install -y python2.7 g++ make
RUN rm -f /usr/bin/python && ln -s /usr/bin/python2.7 /usr/bin/python

# Install Kerberos libs
RUN apt-get install -y libkrb5-dev

# Install Node.js via n
ENV NPM_CONFIG_LOGLEVEL info
RUN git clone https://github.com/tj/n /n
RUN cd /n && make install
RUN n 6.14.4
RUN n 8.12.0
RUN n 10.11.0

RUN mkdir -p /node-krb5
COPY ./run.sh /run.sh
ENTRYPOINT ["/run.sh"]

