FROM alpine:latest

RUN apk add build-base 

COPY ./src ./

RUN chmod +x build.sh && ./build.sh


EXPOSE 8888
CMD ["/bin/sh", "-c" , "./main"]
