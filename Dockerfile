FROM ubuntu:12.04

# install dependancies
WORKDIR /
RUN apt-get update \
    && apt-get install -y gcc-4.4 vim build-essential gdb \
        wget g++ pkg-config zlib1g-dev libglib2.0-dev git\
        libc6-dev autoconf libtool libsdl1.2-dev libx11-dev libxrandr-dev \
        libxi-dev perl qemu\
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.4 10

# install bochs and qemu
RUN mkdir /apps \
    && cd /apps \
    && wget http://download.qemu-project.org/qemu-0.15.0.tar.gz \
    && tar xzf qemu-0.15.0.tar.gz \
    && cd /apps/qemu-0.15.0 \
    && ./configure --target-list=x86_64-softmmu --disable-werror --enable-debug \
    && make \
    && make install \
    && ln -s /apps/qemu/x86_64-softmmu/qemu-x86_64 /bin/qemu \
    && ln -s /apps/qemu/x86_64-softmmu/qemu-x86_64 /bin/qemu-system-i386 \
    && cd /apps \
    && wget http://sourceforge.net/projects/bochs/files/bochs/2.6.2/bochs-2.6.2.tar.gz \
    && tar -xzvf bochs-2.6.2.tar.gz \
    && cd /apps/bochs-2.6.2 \
    && ./configure --enable-gdb-stub --with-nogui \
    && make \
    && make install  



# set envs
ENV PATH="$PATH:/pintos/src/utils" \
    PINTOSPATH="/pintos"

COPY . /pintos

#make bochs
#RUN cd /app/bochs-2.6.2 && \
#    ./configure --enable-gdb-stub --with-nogui && \
#    make clean && \
#    make
 
RUN cd /pintos/src/utils && \
    make clean && \
    make && \
    cd /pintos/src/threads && \
    make clean && \
    make

WORKDIR /pintos/src/

CMD pintos -q run alarm-multiple
    
