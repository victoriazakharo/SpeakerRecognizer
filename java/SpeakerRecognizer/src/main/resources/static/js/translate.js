$(function() {
    function translate(){
        var messages = messageJson[lang];
        $("[data-translate]").each(function(ix, value) {
            var key = value.getAttribute("data-translate");
            value.innerHTML = messages[key];
        });
        waiter.waitMessage = messages["waiting"];
        outputText.prop('placeholder', messages["recognized-speaker"]);
    }
    $.getJSON('messages.json', function(data) {
        messageJson = data;
        translate();
    });
    $("#en").click(function() {
        lang = "en";
        translate();
    });
    $("#be").click(function() {
        lang = "be";
        translate();
    });
});

