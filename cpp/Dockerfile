FROM ubuntu:16.04 as cppbuilder

RUN apt-get update
RUN apt-get -y install build-essential cmake
RUN apt-get -y install liblapack-dev libblas-dev libarmadillo-dev libsndfile-dev libfftw3-dev libmpg123-dev 
RUN apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
COPY / /cpp
WORKDIR /cpp
RUN cmake SpeakerRecognizer
RUN make

FROM ubuntu:16.04

RUN apt-get update
RUN apt-get -y install liblapack3 libblas3 libarmadillo6 libsndfile1 libfftw3-3 libmpg123-0
RUN apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

COPY --from=cppbuilder /cpp/bin/SpeakerRecognizer /SpeakerRecognizer
WORKDIR /

ENTRYPOINT ["./SpeakerRecognizer"]

