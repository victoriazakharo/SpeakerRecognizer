FROM ubuntu:16.04

RUN apt-get update
RUN apt-get -y install liblapack3 libblas3 libarmadillo6
RUN apt-get -y install libsndfile1 libfftw3-3 libmpg123-0

RUN apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

COPY /SpeakerRecognizer /SpeakerRecognizer
WORKDIR /

ENTRYPOINT ["./SpeakerRecognizer"]
