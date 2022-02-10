FROM centos:7

# Install epel (requirement for service nginx)
RUN yum install -y epel-release

RUN yum install -y centos-release-scl
RUN yum install -y devtoolset-10-gcc-c++

# Install misc
RUN yum install -y wget git vim sudo make

# Install compilation tools
RUN yum install -y python3 make

# Install Kerberos libs
RUN yum install -y krb5-devel

# Install Node.js via n
ENV NPM_CONFIG_LOGLEVEL info
RUN git clone https://github.com/tj/n /n
RUN cd /n && make install
RUN n 14
RUN n 16

RUN mkdir -p /node-krb5
COPY ./run.sh /run.sh
ENTRYPOINT ["/run.sh"]
