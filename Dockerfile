FROM ubuntu:20.04

COPY . /vcom
WORKDIR /vcom

RUN apt-get update  && \
 apt-get -y install vim  && \ 
apt install -y gcc make && \
apt-get install -y g++ libstdc++-10-dev && \
apt-get install -y python3.7 
RUN make
