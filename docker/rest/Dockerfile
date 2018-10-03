FROM centos:7

# EPEL
RUN yum install -y epel-release

# Python 3
RUN yum install -y https://centos7.iuscommunity.org/ius-release.rpm
RUN yum update -y && yum install -y python36u python36u-libs python36u-devel python36u-pip

# Install misc
RUN yum install -y wget git vim sudo gcc

# Install Kerberos
RUN yum install -y krb5-devel krb5-workstation

RUN pip3.6 install --upgrade pip
RUN pip3.6 install gssapi

RUN mkdir -p /node-krb5
COPY ./run.sh /run.sh
RUN chmod +x /run.sh

ENTRYPOINT ["/run.sh"]
