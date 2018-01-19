# Version: 0.0.1
FROM ubuntu:17.10
MAINTAINER Ilya Petukhov <iproha94@tarantool.org>

RUN apt-get update

RUN apt-get install -y -f \
    numactl gcc libc6-dev zlib1g-dev make libmysqlclient-dev \
    ssh vim git dh-autoreconf pkg-config libicu-dev \
    build-essential cmake coreutils sed libreadline-dev \
    libncurses5-dev libyaml-dev libssl-dev libcurl4-openssl-dev \
    libunwind-dev python python-pip python-setuptools python-dev \
    python-msgpack python-yaml python-argparse python-six python-gevent

# download, build, install tarantool
RUN git clone --recursive https://github.com/tarantool/tarantool.git \
    -b 1.8 /opt/tarantool
WORKDIR /opt/tarantool
RUN cmake . -DENABLE_DIST=ON  -DCMAKE_BUILD_TYPE=RelWithDebInfo && \
    make; make install

# download, build, install tarantool-c
RUN git clone --recursive https://github.com/tarantool/tarantool-c.git \
    /opt/tarantool-c
WORKDIR /opt/tarantool-c/third_party/msgpuck/
RUN cmake . ; make; make install
WORKDIR /opt/tarantool-c
RUN cmake . ; make; make install

# download, build, install msgpack-c
RUN git clone https://github.com/msgpack/msgpack-c.git \
    /opt/msgpack-c
WORKDIR /opt/msgpack-c
RUN cmake . ; make ; make install

# download, build, install tpcc
COPY . /opt/tpcc
WORKDIR /opt/tpcc/src
RUN make

WORKDIR /opt/tpcc
CMD ["./benchmarking/run-from-docker.sh"]