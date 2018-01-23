FROM ubuntu:16.04

RUN apt-get update
RUN apt-get install -y openjdk-8-jdk gradle
COPY /SpeakerRecognizer /SpeakerRecognizer
WORKDIR /SpeakerRecognizer
RUN gradle build

RUN apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
