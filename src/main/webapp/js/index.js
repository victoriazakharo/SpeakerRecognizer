'use strict'

function setLocale(locale) {
    window.location.href = window.location.pathname + "?lang=" + locale.substr(0,2);
}

function selectUploadFileTab() {
    var type = window.location.hash.substr(1);
    if (type == 'uploadtab') {
        $('#exampletab').removeClass('in active');
        $('#tabExample').removeClass('active');
    }
}

$(document).ready(function() {
    $('.setLocale').click(function(e) {
        e.preventDefault();
        var localeId = $( this ).attr('href');
        switch (localeId) {
            case "#En":
                setLocale("en_US.UTF-8");
                break;
            default:
                setLocale("en_US.UTF-8");
                break;
        }
    });

    $('#exampleFile').change(function(e) {
        e.preventDefault();
        var exampleSource = this.value;
        $('#exampleSource').attr("src", "wav/" + exampleSource);

        var player = $('#player');
        player[0].pause();
        player[0].load();
    });

    $('#exampleBtn').click(function(e) {
        e.preventDefault();
        $('#outputText').val("Somebody");
    });

    navigator.mediaDevices.getUserMedia = (navigator.mediaDevices.getUserMedia ||
        navigator.webkitGetUserMedia ||
        navigator.mozGetUserMedia ||
        navigator.msGetUserMedia);
});