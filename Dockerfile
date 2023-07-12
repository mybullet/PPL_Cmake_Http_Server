#
FROM ubuntu:latest

RUN apt-get update && apt-get install -y apt-utils

#所需的软件包和依赖项
RUN apt-get update && apt-get install -y \
    libboost-system-dev \
    libboost-filesystem-dev \
    libboost-thread-dev \
    libboost-regex-dev \
    libboost-chrono-dev \
    libmysqlclient-dev \
    libzmq3-dev \
    build-essential \
    cmake \
    netcat



EXPOSE 10000
EXPOSE 3306

COPY ./HttpServer /app/HttpServer

# WORKDIR /app
# RUN mkdir tmp && cd /app/tmp && touch test.ipc && chmod 777 test.ipc
# RUN mkdir tmp 
# ENV ZMQ_IPC_FILE /app/tmp/test.ipc

WORKDIR /app/HttpServer/build
RUN cmake .. && make

# 设置工作目录
WORKDIR /app/HttpServer/build

# 定义容器启动时执行的命令
CMD ["./start_http_server.sh"]
