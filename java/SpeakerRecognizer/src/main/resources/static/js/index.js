'use strict'

function setLocale(locale) {
    window.location.href = window.location.pathname + "?lang=" + locale.substr(0, 2);
}

$(document).ready(function () {
    var testFileSelect = $('#testFiles');
    var selectedRecord = $('#testSource');
    var player = $('#player');
    $.ajax({
        type: "GET",
        url: "getTestRecords",
        success: function (data) {
            for (var i = 0; i < data.length; i++) {
                testFileSelect.append($('<option>', { value: data[i] }).text(data[i]));
            }
            if (data.length > 0) {
                selectedRecord.attr("src", "audio/test/" + data[0]);
                player[0].load();
            }
        }
    });
    $.ajax({
        type: "GET",
        url: "getSpeakerRecords",
        success: function (data) {
            for (var i = 0; i < data.length; i++) {
                var audio = $("<audio>");
                audio.attr({
                    "src": "audio/example/" + data[i][0],
                    "controls": ""
                });
                var row = $("<tr>");
                var name = $("<td>").append(data[i][1]).addClass("col-xs-2");
                var record = $("<td>").append(audio).addClass("col-xs-10");
                row.append(name).append(record);
                $("#speakerFiles").append(row);
            }
        }
    });

    $('.setLocale').click(function (e) {
        e.preventDefault();
        var localeId = $(this).attr('href');
        switch (localeId) {
            case "#En":
                setLocale("en_US.UTF-8");
                break;
            default:
                setLocale("en_US.UTF-8");
                break;
        }
    });

    testFileSelect.change(function (e) {
        e.preventDefault();
        var selectedSource = this.value;
        selectedRecord.attr("src", "audio/test/" + selectedSource);
        player[0].pause();
        player[0].load();
    });

    $('#testBtn').click(function (e) {
        e.preventDefault();
        $('#testBtn').prop('disabled', true);
        $('#outputText').val("Waiting...");
        var src = selectedRecord.attr("src");
        $.ajax({
            type: "GET",
            url: "recognize",
            data: { "path": src },
            success: function (data) {
                $('#outputText').val(data);
                $('#testBtn').prop('disabled', false);
            }
        });
    });

    var context;
    var recorder;
    var localStream;
    var recordBtn = $('#record');
    var stopBtn = $('#stop');
    var recognizeRecordedBtn = $('#recognizeRecordedBtn');

    recognizeRecordedBtn.prop('disabled', true);
    stopBtn.prop('disabled', true);
    stopBtn.hide();

    navigator.mediaDevices.getUserMedia = (navigator.mediaDevices.getUserMedia ||
        navigator.webkitGetUserMedia || navigator.mozGetUserMedia || navigator.msGetUserMedia);

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

    recordBtn.click(function() {
        navigator.getUserMedia(
            {audio: true},
            onGetUserMedia,
            function () {
                console.warn("Error getting audio stream from getUserMedia");
            });
        recognizeRecordedBtn.prop('disabled', true);
        recordBtn.prop('disabled', true);
        stopBtn.prop('disabled', false);
        recordBtn.hide();
        stopBtn.show();
    });

    stopBtn.click(function() {
        recordBtn.prop('disabled', false);
        recognizeRecordedBtn.prop('disabled', false);
        stopBtn.prop('disabled', true);
        recordBtn.show();
        stopBtn.hide();

        recorder && recorder.exportWAV(function(blob) {
            var url = URL.createObjectURL(blob);
            $('#recorderSource').attr("src", url);
            var player = $('#recorderPlayer');
            player[0].pause();
            player[0].load();
        });

        recorder.stop();
        localStream.getTracks()[0].stop();
        context.disabled = true;
        context.close();
    });

    recognizeRecordedBtn.click(function(e) {
        e.preventDefault();
        recognizeRecordedBtn.prop('disabled', true);
        $('#outputText').val("Waiting...");
        recorder && recorder.exportWAV(function(blob) {
            var fd = new FormData();
            fd.set("data", blob, "uploaded.wav");
            $.post({
                url: 'recognizeUploaded',
                data: fd,
                enctype: 'multipart/form-data',
                processData: false,
                contentType: false,
                cache: false,
            }).done(function(data) {
                recognizeRecordedBtn.prop('disabled', false);
                $('#outputText').val(data);
            });
        });
    });
});