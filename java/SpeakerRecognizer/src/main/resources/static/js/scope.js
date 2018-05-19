var uploadForm = $("#uploadForm");
var uploadBtn = $('#uploadBtn');
var testBtn = $('#testBtn');
var context;
var recorder;
var localStream;
var exampleAudio = document.getElementById('exampleAudio');
var recognizeRecordedBtn = $('#recognizeRecordedBtn');
var enrollBtn = $('#enrollBtn');
var outputText = $('#outputText');
var outputPanel = $("#outputPanel");
var exampleAudioSource = $('#exampleAudioSource');
var exampleSource = $('#exampleSource');
var speakersSource = $('#speakersSource');
var uploadSource = $('#uploadSource');
var recordSource = $('#recordSource');
var enrollSource = $('#enrollSource');
var waiter = {
    actionButtons: [recognizeRecordedBtn, uploadBtn, testBtn],
    waitMessage: "",
    start: function(){
        outputText.html(this.waitMessage);
        for (var i = 0; i < this.actionButtons.length; i++) {
            this.actionButtons[i].prop('disabled', true);
        }
    },
    stop: function(lang){
        for (var i = 0; i < this.actionButtons.length; i++) {
            this.actionButtons[i].prop('disabled', false);
        }
    }
}
var lang = "en";
var messageJson = {};
var enrolledRecords = {};
