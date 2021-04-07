FROM ubuntu:12.04

# install dependancies
RUN apt-get update && \
    apt-get install -y gcc-4.4 vim build-essential qemu gdb && \
    mv /usr/bin/gcc-4.4 /usr/bin/gcc

# set envs
ENV PATH="$PATH:/app/src/utils" \
    PINTOSPATH="/app"

COPY . /app

# make bochs
# RUN cd /app/bochs-2.6.2 && \
    # ./configure --enable-gdb-stub --with-nogui && \
    # make clean && \
    # make
 
RUN cd /app/src/utils && \
    make clean && \
    make && \
    cd /app/src/threads && \
    make clean && \
    make

WORKDIR /app/src/

CMD pintos -q run alarm-multiple
    
