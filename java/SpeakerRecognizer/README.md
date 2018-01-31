# SpeakerRecognizer

## Startup commands
### IDE run
gradle bootRun -Dspeakers.directory=path/to/directory/
### WAR packaging
gradle build
### Packaged WAR run
java -Dspeakers.directory=path/to/directory/ -jar build/libs/speaker-recognizer-0.1.war
