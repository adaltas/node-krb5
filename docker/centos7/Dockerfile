FROM centos:7

# Install epel (requirement for service nginx)
RUN yum install -y epel-release

# Install misc
RUN yum install -y wget git vim sudo make

# Install compilation tools
RUN yum install -y python2 gcc-c++ make

# Install Kerberos libs
RUN yum install -y krb5-devel

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
