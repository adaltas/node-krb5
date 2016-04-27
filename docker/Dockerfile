# docker build -t pierrotws/nodekrb5
FROM centos:centos7
RUN yum -y update
RUN yum -y install epel-release glibc gcc gcc-c++ krb5-server krb5-workstation krb5-libs krb5-devel
RUN yum -y install nodejs
RUN yum -y install npm
RUN npm install -g node-gyp
