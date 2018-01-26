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

    navigator.mediaDevices.getUserMedia = (navigator.mediaDevices.getUserMedia ||
        navigator.webkitGetUserMedia || navigator.mozGetUserMedia || navigator.msGetUserMedia);
});