function bindEnrollButtons(recordBtn, stopBtn, actionBtn){
    actionBtn.prop('disabled', true);
    stopBtn.hide();
    recordBtn.click(function() {
        startRecord();
        $(".btn-danger").prop('disabled', true);
        $(".btn-warning").prop('disabled', true);
        recordBtn.hide();
        stopBtn.show();
    });
    stopBtn.click(function() {
        actionBtn.prop('disabled', false);
        $(".btn-danger").prop('disabled', false);
        recordBtn.show();
        stopBtn.hide();
        stopRecord();
    });
}

function setTextToEnroll(source){
    let source_lang = source.split('_')[0];
    let text = $("#textToEnroll");
    text.empty();
    $.getJSON("texts/" + source_lang + ".json", function(messages) {
        for(let i = 0; i < messages.length; i++){
            let row = $("<p>").addClass("row text-fragment");
            let recordButton = $("<button>").addClass("btn btn-danger")
                .attr("data-translate", "record").text(messageJson[lang]["record"]);
            let stopButton = $("<button>").addClass("btn btn-info")
                .attr("data-translate", "stop").text(messageJson[lang]["stop"]).hide();
            let okButton = $("<button>").addClass("btn btn-success")
                .append($("<span>").addClass("glyphicon glyphicon-ok"));
            let recordId = i + "";
            if(enrolledRecords[source].indexOf(recordId) < 0){
                okButton.hide();
            }
            let processButton = $("<button>").addClass("btn btn-warning").attr("id", recordId)
                .attr("data-translate", "process").text(messageJson[lang]["process"]);
            processButton.click(function (e) {
                e.preventDefault();
                waiter.start();
                let source = enrollSource.find(":selected").val();
                let id = $(this).attr('id');
                recorder && recorder.exportWAV(function (blob) {
                    let fd = new FormData();
                    fd.set("data", blob, id + ".wav");
                    fd.set("source", source);
                    $.post({
                        url: 'process',
                        data: fd,
                        enctype: 'multipart/form-data',
                        processData: false,
                        contentType: false,
                        cache: false,
                    }).done(function (data) {
                        outputText.html(messageJson[lang]["not-enrolled"]);
                        processButton.prop('disabled', true);
                        if(data == "accepted"){
                            okButton.show();
                            enrolledRecords[source].push(id);
                            if(enrolledRecords[source].length == messages.length){
                                enrollBtn.prop('disabled', false);
                            }
                        } else {
                            okButton.hide();
                        }
                        waiter.stop();
                    });
                });
            });
            let name = $("<i>").append(messages[i]).addClass("col-xs-8");
            let record = $("<div>").append(recordButton).append(stopButton).
                append(processButton).append(okButton).addClass("col-xs-4");
            bindEnrollButtons(recordButton, stopButton, processButton);
            row.append(name).append(record);
            text.append(row);
        }
    });
}

$(function() {
    $.get("getUserInfo", function (data) {
        enrolledRecords = data;
    });
    enrollBtn.prop('disabled', true);
    enrollBtn.click(function (e) {
        e.preventDefault();
        waiter.start();
        waiter.stop();
    });
});