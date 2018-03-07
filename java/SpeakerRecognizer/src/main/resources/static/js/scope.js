var uploadForm = $("#uploadForm");
var uploadBtn = $('#uploadBtn');
var testBtn = $('#testBtn');
var context;
var recorder;
var localStream;
var recordBtn = $('#record');
var stopBtn = $('#stop');
var player = document.getElementById('player');
var recognizeRecordedBtn = $('#recognizeRecordedBtn');
var outputText = $('#outputText');
var exampleSource = $('#exampleSource');
var speakersSource = $('#speakersSource');
var uploadedSource = $('#uploadedSource');
var recordedSource = $('#recordedSource');
var waiter = {
    actionButtons: [recognizeRecordedBtn, uploadBtn, testBtn],
    waitMessage: "",
    start: function(){
        $('#outputText').val(this.waitMessage);
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