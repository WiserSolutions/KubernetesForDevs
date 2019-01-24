FROM alpine:latest
RUN apk add --no-cache gcc libc-dev
COPY . /work/ipc
WORKDIR /work/ipc
RUN gcc -o ipc ipc.c -lrt
ENTRYPOINT ["./ipc"]