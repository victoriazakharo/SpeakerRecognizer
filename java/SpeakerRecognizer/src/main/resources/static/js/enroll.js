function userEnrolled(source){
    return enrolledRecords[source].length == enrollRecordNumbers[source];
}

function updateEnrollState(source) {
    let enrolled = userEnrolled(source);
    enrollBtn.prop('disabled', !enrolled);
    if (enrolled) {
        outputText.html(messageJson[lang]["enrolled"]);
        outputText.attr('data-translate', "enrolled");
    } else {
        outputText.html(messageJson[lang]["not-enrolled"]);
        outputText.attr('data-translate', "not-enrolled");
    }
}

function buildEnrollTable(exampleAudio, userAudio){
    let exampleRow = $("<tr>")
        .append($("<th scope='row' data-translate='example'>").text(messageJson[lang]["example"]))
        .append($("<td>").append(exampleAudio));
    let userRow = $("<tr>")
        .append($("<th scope='row' data-translate='you'>").text(messageJson[lang]["you"]))
        .append($("<td>").append(userAudio));
    let tableBody = $("<tbody>").append(exampleRow).append(userRow);
    return $("<table>").addClass("table").append(tableBody);
}

function getExampleAudio(source, recordId){
    let exampleSource = $("<source type='audio/wav' data-translate='audio-unsupported'>")
        .attr("src", source + "/audio/enroll/" + recordId + ".wav");
    return $("<audio controls=''>").append(exampleSource);
}

function getUserAudio(){
    let userSource =  $("<source src='' type='audio/wav' data-translate='audio-unsupported'>")
    return $("<audio controls=''>").append(userSource);
}

function setTextToEnroll(source){
    let source_lang = source.split('_')[0];
    let text = $("#textToEnroll");
    text.empty();
    $.getJSON("texts/" + source_lang + ".json", function(messages) {
        enrollRecordNumbers[source] = messages.length;
        updateEnrollState(source);
        for(let i = 0; i < messages.length; i++){
            let row = $("<p>").addClass("row text-fragment");
            let recordButton = $("<button>").addClass("btn btn-danger record-button")
                .attr("data-translate", "record").text(messageJson[lang]["record"]);
            let stopButton = $("<button>").addClass("btn btn-info")
                .attr("data-translate", "stop").text(messageJson[lang]["stop"]).hide();
            let okButton = $("<button>").addClass("btn btn-success")
                .append($("<span>").addClass("glyphicon glyphicon-ok"));
            let failButton = $("<button>").addClass("btn btn-danger")
                .append($("<span>").addClass("glyphicon glyphicon-remove"));
            let recordId = i + "";
            failButton.hide();
            if(!enrolledRecords[source].has(recordId)){
                okButton.hide();
            }
            let processButton = $("<button>")
                .addClass("btn btn-warning process-button").attr("id", recordId)
                .attr("data-translate", "process").text(messageJson[lang]["process"]);
            processButton.click(function(){
                onProcess(okButton, failButton, processButton);
            });
            let name = $("<i>").append(messages[i]).addClass("col-xs-8");
            let exampleAudio = getExampleAudio(source, recordId);
            let userAudio = getUserAudio();
            let table = buildEnrollTable(exampleAudio, userAudio);
            let record = $("<div>").append(recordButton).append(stopButton).
                append(processButton).append(okButton).append(failButton).append(table).addClass("col-xs-4");
            bindRecordButtons(recordButton, stopButton, userAudio, processButton, true);
            row.append(name).append(record);
            text.append(row);
        }
    });
}

function process(okButton, failButton, processButton, source, data) {
    outputText.html(messageJson[lang]["not-enrolled"]);
    processButton.prop('disabled', true);
    if (data == "accepted") {
        okButton.show();
        failButton.hide();
        enrolledRecords[source].add(id);
    } else {
        okButton.hide();
        failButton.show();
    }
    updateEnrollState(source);
    waiter.stop();
}

function onProcess(okButton, failButton, processButton){
    waiter.start();
    let source = enrollSource.find(":selected").val();
    let id = processButton.attr('id');
    recorder && recorder.exportWAV(function (blob) {
        if (blob.size > 3000000) {
            $("#tabs-alert").removeClass("in").show();
            $("#tabs-alert").delay(200).addClass("in").fadeOut(2000);
            $("#alert-text").html(messageJson[lang]["record-too-long"]);
            outputText.html(messageJson[lang]["not-enrolled"]);
            process(okButton, failButton, processButton, source, "failed");
            return;
        }
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
            process(okButton, failButton, processButton, source, data);
        });
    });
}

$(function() {
    $.get("getUserInfo", function (data) {
        Object.keys(data).forEach(function(key) {
            enrolledRecords[key] = new Set(data[key]);
        });
    });
    enrollBtn.prop('disabled', true);
    enrollBtn.click(function (e) {
        e.preventDefault();
        waiter.start();
        let source = enrollSource.find(":selected").val();
        $.get("add", { "source": source }, function () {
            outputText.html(messageJson[lang]["enrolled"]);
            waiter.stop();
        });
    });
});