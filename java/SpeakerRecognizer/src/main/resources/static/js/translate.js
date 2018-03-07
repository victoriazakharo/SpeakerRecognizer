$(function() {
    function translate(lang){
        $.getJSON('messages.json', function(data) {
            var messages = data[lang];
            $("[data-translate]").each(function(ix, value) {
                var key = value.getAttribute("data-translate");
                value.innerHTML = messages[key];
            });
            waiter.waitMessage = messages["waiting"];
        });
    }

    translate("en");
    $("#en").click(function() {
        translate("en");
    });
    $("#be").click(function() {
        translate("be");
    });
    $('#outputPanel').on("DOMSubtreeModified", function(){
        $('#outputText').prop('placeholder', $('#outputPanel').text());
    });
});

