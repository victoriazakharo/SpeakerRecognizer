function onGetUserMedia(stream) {
    localStream = stream;
    context = new AudioContext();
    var mediaStreamSource = context.createMediaStreamSource(stream);
    var config = {
        bufferLen: 4096,
        numChannels: 1,
        mimeType: 'audio/wav'
    };
    recorder = new Recorder(mediaStreamSource, config);
    recorder.record();
}

function startRecord(){
    navigator.getUserMedia(
        {audio: true},
        onGetUserMedia,
        function () {
            console.warn("Error getting audio stream from getUserMedia");
        });
}

function stopRecord() {
    if(recorder){
        recorder.stop();
    }
    if(localStream){
        localStream.getTracks()[0].stop();
    }
    if(context) {
        context.disabled = true;
        context.close();
    }
}

function setTextToRecognize(source){
    var text = source.startsWith("be_") ?
        "У рудога вераб’я ў сховішчы пад фатэлем ляжаць нейкія гаючыя зёлкі." :
        "The quick brown fox jumps over the lazy dog.";
    $("#textToRecognize").text(text);
}

function bindRecordButtons(recordBtn, stopBtn, audio, actionBtn, triggerNeighbors){
    actionBtn.prop('disabled', true);
    stopBtn.hide();
    recordBtn.click(function() {
        startRecord();
        actionBtn.prop('disabled', true);
        if(triggerNeighbors){
            $(".record-button, .process-button").prop('disabled', true);
        }
        recordBtn.hide();
        stopBtn.show();
    });
    let audioElement = audio[0];
    stopBtn.click(function() {
        actionBtn.prop('disabled', false);
        if(triggerNeighbors) {
            $(".record-button").prop('disabled', false);
        }
        recordBtn.show();
        stopBtn.hide();
        if(recorder){
            recorder.exportWAV(function(blob) {
                var url = URL.createObjectURL(blob);
                audioElement.children[0].setAttribute("src", url);
                audioElement.pause();
                audioElement.load();
            });
        }
        stopRecord();
    });
}

$(function() {
    bindRecordButtons($("#recordTabRecord"), $("#recordTabStop"), $("#recordAudio"),
        recognizeRecordedBtn, false);
    recognizeRecordedBtn.click(function(e) {
        e.preventDefault();
        waiter.start();
        var source = recordSource.find(":selected").val();
        recorder && recorder.exportWAV(function(blob) {
            var fd = new FormData();
            fd.set("data", blob, "uploaded.wav");
            fd.set("source", source);
            $.post({
                url: 'recognizeUploaded',
                data: fd,
                enctype: 'multipart/form-data',
                processData: false,
                contentType: false,
                cache: false,
            }).done(function(data) {
                if(data == "you"){
                    data = messageJson[lang][data];
                    outputText.attr('data-translate', "you");
                }
                outputText.html(data);
                waiter.stop();
            });
        });
    });
});