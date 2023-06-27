ARG PROJECT

FROM ubuntu:20.04 as libadore_external_library_builder

ARG PROJECT
ARG REQUIREMENTS_FILE="requirements.${PROJECT}.ubuntu20.04.system"


RUN mkdir -p /tmp/${PROJECT}
COPY files/${REQUIREMENTS_FILE} /tmp/${PROJECT}

WORKDIR /tmp/${PROJECT}

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install --no-install-recommends -y checkinstall && \
    apt-get install --no-install-recommends -y apt-transport-https ca-certificates gnupg software-properties-common wget && \
    xargs apt-get install --no-install-recommends -y < ${REQUIREMENTS_FILE} && \
    rm -rf /var/lib/apt/lists/*

RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null

RUN echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ focal main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null && \
    apt-get update && \
    apt-get install -y --no-install-recommends kitware-archive-keyring && \
    apt-get install -y --no-install-recommends cmake

#RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | apt-key add - && \
#    echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ focal main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null && \
#    rm -f /usr/share/keyrings/kitware-archive-keyring.gpg && \
#    apt-get update && apt-get install -y --no-install-recommends kitware-archive-keyring && \
#    apt-get update && apt-get install -y --no-install-recommends cmake 


COPY ${PROJECT} /tmp/${PROJECT}/

WORKDIR /tmp/${PROJECT}
RUN mkdir -p build
WORKDIR /tmp/${PROJECT}/build

RUN cmake .. -DCMAKE_INSTALL_PREFIX:PATH=install && \
    cmake --build . -- -j $(nproc) && \
    cmake --install .

RUN ls -la /tmp/${PROJECT}/build
RUN cpack -G DEB && find . -type f -name "*.deb" | xargs mv -t . || true

RUN mv CMakeCache.txt CMakeCache.txt.build

FROM alpine:3.14

ARG PROJECT

COPY --from=libadore_external_library_builder /tmp/${PROJECT} /tmp/${PROJECT}

