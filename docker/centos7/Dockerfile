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
run n 6.17.1
run n 8.16.1
run n 10.16.3
run n 12.10.0

RUN mkdir -p /node-krb5
COPY ./run.sh /run.sh
ENTRYPOINT ["/run.sh"]
