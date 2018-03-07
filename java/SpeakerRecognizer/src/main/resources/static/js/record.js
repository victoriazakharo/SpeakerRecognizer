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

$(function() {
    recognizeRecordedBtn.prop('disabled', true);
    stopBtn.prop('disabled', true);
    stopBtn.hide();

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

        if(recorder){
            recorder.exportWAV(function(blob) {
                var url = URL.createObjectURL(blob);
                $('#recorderSource').attr("src", url);
                var player = $('#recorderPlayer');
                player[0].pause();
                player[0].load();
            });
            recorder.stop();
        }
        localStream.getTracks()[0].stop();
        context.disabled = true;
        context.close();
    });

    recognizeRecordedBtn.click(function(e) {
        e.preventDefault();
        waiter.start();
        var source = recordedSource.find(":selected").val();
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
                outputText.val(data);
                waiter.stop();
            });
        });
    });
});