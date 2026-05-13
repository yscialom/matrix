FROM alpine:3.21

RUN apk update && apk add clang18-extra-tools=18.1.8-r2

WORKDIR /mnt
ENTRYPOINT ["/usr/lib/llvm18/bin/clang-format"]
