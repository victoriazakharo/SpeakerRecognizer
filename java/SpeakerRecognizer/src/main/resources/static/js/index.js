'use strict'

function fillTest(source) {
    var testFileSelect = $('#testFiles');
    testFileSelect.empty();
    $.ajax({
        type: "GET",
        url: "getTestRecords",
        data: { "source": source },
        success: function (data) {
            for (var i = 0; i < data.length; i++) {
                testFileSelect.append($('<option>', { value: data[i] }).text(data[i]));
            }
            if (data.length > 0) {
                $('#testSource').attr("src", source + "/audio/test/" + data[0]);
                player.load();
            }
        }
    });
}

function fillSpeakers(source){
    var speakers = $("#speakerFiles");
    speakers.empty();
    $.ajax({
        type: "GET",
        url: "getSpeakerRecords",
        data: { "source": source },
        success: function (data) {
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
        }
    });
}

$(function() {
    exampleSource.change(function () {
        fillTest(this.value);
    });
    speakersSource.change(function () {
        fillSpeakers(this.value);
    });
    $.ajax({
        type: "GET",
        url: "getSources",
        success: function (data) {
            var sources = [
                exampleSource,
                uploadedSource,
                recordedSource,
                speakersSource
            ];
            for(var j = 0; j < sources.length; j++){
                for(var i = 0; i < data.length; i++){
                    var html = i ? "<option />" : "<option selected/>";
                    sources[j].append($(html).val(data[i]).text(data[i]));
                }
                sources[j].selectpicker("refresh");
                sources[j].trigger("change");
            }
        }
    });
    $('#testFiles').change(function(e) {
        e.preventDefault();
        var source = exampleSource.find(":selected").val();
        $('#testSource').attr("src", source + "/audio/test/" + this.value);
        player.pause();
        player.load();
    });

    testBtn.click(function (e) {
        e.preventDefault();
        var source = exampleSource.find(":selected").val();
        waiter.start();
        var src = $('#testSource').attr("src");
        $.ajax({
            type: "GET",
            url: "recognize",
            data: { "source": source, "path": src },
            success: function (data) {
                outputText.val(data);
                waiter.stop();
            }
        });
    });
    navigator.mediaDevices.getUserMedia = (navigator.mediaDevices.getUserMedia ||
        navigator.webkitGetUserMedia || navigator.mozGetUserMedia || navigator.msGetUserMedia);
});