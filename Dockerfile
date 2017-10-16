# Version: 0.0.1
FROM ubuntu
MAINTAINER Ilya Petukhov <iproha94@tarantool.com>

RUN mkdir snapshot
COPY snapshot/ snapshot/

RUN apt-get update

RUN apt-get install -y -f gcc libc6-dev zlib1g-dev make libmysqlclient-dev
RUN apt-get install -y -f ssh vim git dh-autoreconf pkg-config
RUN apt-get install -y -f build-essential cmake coreutils sed libreadline-dev \
      libncurses5-dev libyaml-dev libssl-dev libcurl4-openssl-dev \
      libunwind-dev python python-pip python-setuptools python-dev \
      python-msgpack python-yaml python-argparse python-six python-gevent

RUN git clone https://github.com/msgpack/msgpack-c.git
RUN cd msgpack-c ; cmake . ; make ; make install ; cd ..