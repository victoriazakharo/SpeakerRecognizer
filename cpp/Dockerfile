FROM ubuntu:16.04

RUN apt-get update
RUN apt-get -y install build-essential cmake
RUN apt-get -y install liblapack-dev libblas-dev libarmadillo-dev
RUN apt-get -y install libsndfile-dev libfftw3-dev libmpg123-dev 
COPY / /cpp
WORKDIR /cpp
RUN cmake SpeakerRecognizer
RUN make
RUN apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
