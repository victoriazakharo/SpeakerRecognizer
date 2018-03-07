FROM ubuntu:16.04 as javabuilder

RUN apt-get update && apt-get install -y openjdk-8-jdk gradle
RUN apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
COPY /SpeakerRecognizer /SpeakerRecognizer
WORKDIR /SpeakerRecognizer
RUN gradle build


FROM ubuntu:16.04

RUN apt-get update && apt-get install -y openjdk-8-jre sox
RUN apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

COPY --from=javabuilder /SpeakerRecognizer/build/libs/speaker-recognizer-0.1.war /speaker-recognizer-0.1.war
WORKDIR /

ENTRYPOINT ["java", "-jar", "speaker-recognizer-0.1.war"]
