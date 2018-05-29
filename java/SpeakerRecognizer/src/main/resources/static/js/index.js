'use strict'

function fillTest(source) {
    var testFileSelect = $('#testFiles');
    testFileSelect.empty();
    $.get("getTestRecords", { "source": source },
        function (data) {
            for (var i = 0; i < data.length; i++) {
                testFileSelect.append($('<option>', { value: data[i] }).text(data[i]));
            }
            if (data.length > 0) {
                exampleAudioSource.attr("src", source + "/audio/test/" + data[0]);
                exampleAudio.load();
            }
        });
}

function fillSpeakers(source){
    var speakers = $("#speakerFiles");
    speakers.empty();
    $.get("getSpeakerRecords", { "source": source },
        function (data) {
            for (var i = 0; i < data.length; i++) {
                var audio = $("<audio>");
                audio.attr({
                    "src": source + "/audio/example/" + data[i][0],
                    "controls": ""
                });
                var row = $("<tr>");
                var name = $("<td>").append(data[i][1]).addClass("col-xs-2");
                var record = $("<td>").append(audio).addClass("col-xs-10");
                row.append(name).append(record);
                speakers.append(row);
            }
        });
}

function setRecognizedSpeakerPanel(){
    outputText.html('');
    outputText.removeAttr('data-translate');
    outputPanel.text(messageJson[lang]["recognized-speaker"]);
    outputPanel.attr('data-translate', "recognized-speaker");
}

function initTabs(){
    $("#tabExample").click(function() {
        setRecognizedSpeakerPanel();
        exampleSource.trigger("change");
    });
    $("#tabRecord").click(function() {
        setRecognizedSpeakerPanel();
        recordSource.trigger("change");
    });
    $("#tabUpload").click(setRecognizedSpeakerPanel);
    $("#tabSpeakers").click(function() {
        speakersSource.trigger("change");
    });
    $("#tabEnroll").click(function(){
        outputPanel.text(messageJson[lang]["status"]);
        outputPanel.attr('data-translate', "status");
        enrollSource.trigger("change");
    });
}

$(function() {
    exampleSource.change(function () {
        fillTest(this.value);
    });
    speakersSource.change(function () {
        fillSpeakers(this.value);
    });
    initTabs();
    $.get("getSources", function (data) {
            var sources = [
                exampleSource,
                uploadSource,
                recordSource,
                enrollSource,
                speakersSource
            ];
            for(var j = 0; j < sources.length; j++){
                for(var i = 0; i < data.length; i++){
                    var html = i ? "<option />" : "<option selected/>";
                    sources[j].append($(html).val(data[i]).text(data[i]));
                }
                sources[j].selectpicker("refresh");
            }
        });
    $('#testFiles').change(function(e) {
        e.preventDefault();
        var source = exampleSource.find(":selected").val();
        exampleAudioSource.attr("src", source + "/audio/test/" + this.value);
        exampleAudio.pause();
        exampleAudio.load();
    });

    testBtn.click(function (e) {
        e.preventDefault();
        var source = exampleSource.find(":selected").val();
        waiter.start();
        var src = exampleAudioSource.attr("src");
        $.get("recognize", { "source": source, "path": src }, function (data) {
                outputText.html(data);
                waiter.stop();
            });
    });
    navigator.mediaDevices.getUserMedia = (navigator.mediaDevices.getUserMedia ||
        navigator.webkitGetUserMedia || navigator.mozGetUserMedia || navigator.msGetUserMedia);
});