FROM ubuntu:22.04

RUN apt update -y && apt install -y build-essential libpcap-dev tzdata

ENV WD /src
WORKDIR ${WD}

COPY Makefile .
COPY src/ ${WD}/src/

RUN make build

# Set the timezone to Asia/Ho_Chi_Minh
RUN ln -fs /usr/share/zoneinfo/Asia/Ho_Chi_Minh /etc/localtime && dpkg-reconfigure -f noninteractive tzdata


ENTRYPOINT [ "/src/build/main.out", "0", "tcp", "1000" ]
