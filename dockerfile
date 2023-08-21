FROM debian:latest

RUN apt-get update && apt-get install -y build-essential libmicrohttpd-dev uuid-dev

COPY ./src ./

RUN chmod +x build.sh && ./build.sh

EXPOSE 8888
CMD ["/bin/sh", "-c" , "./main"]
