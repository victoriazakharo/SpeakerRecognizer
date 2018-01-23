FROM ubuntu:16.04

RUN apt-get update
RUN apt-get install -y openjdk-8-jre 
RUN apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

COPY /speaker-recognizer-0.1.war /speaker-recognizer-0.1.war
WORKDIR /

ENTRYPOINT ["java", "-jar", "speaker-recognizer-0.1.war"]
