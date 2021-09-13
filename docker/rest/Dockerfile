FROM centos:7

# EPEL
RUN yum install -y epel-release

# Python 3
RUN yum update -y && yum install -y python3 python3-libs python3-devel python3-pip

# Install misc
RUN yum install -y wget git vim sudo gcc

# Install Kerberos
RUN yum install -y krb5-devel krb5-workstation

RUN pip3 install --upgrade pip
RUN pip3 install gssapi

RUN mkdir -p /node-krb5
COPY ./run.sh /run.sh
RUN chmod +x /run.sh

ENTRYPOINT ["/run.sh"]
