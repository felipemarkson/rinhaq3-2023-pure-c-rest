FROM alpine:latest

RUN apk add build-base libmicrohttpd-dev

COPY ./src ./

RUN chmod +x build.sh && ./build.sh -D


EXPOSE 8888
CMD ["/bin/sh", "-c" , "./main"]
