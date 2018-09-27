FROM centos:7

# Install Node.js
ENV NPM_CONFIG_LOGLEVEL info
ENV NODE_VERSION 9.4.0
RUN yum install -y xz \
  && curl -SLO "https://nodejs.org/dist/v$NODE_VERSION/node-v$NODE_VERSION-linux-x64.tar.xz" \
  && tar -xJf "node-v$NODE_VERSION-linux-x64.tar.xz" -C /usr/local --strip-components=1 \
  && rm -f "/node-v$NODE_VERSION-linux-x64.tar.xz"

# Install epel (requirement for service nginx)
RUN yum install -y epel-release

# Install misc
RUN yum install -y wget git vim sudo

# Install compilation tools
RUN yum install -y python2 gcc-c++ bison make

RUN mkdir -p /node-krb5
ADD ./run.sh /run.sh
ENTRYPOINT ["/run.sh"]
