$(function() {
    $(document).on('change', ':file', function() {
        var input = $(this),
            label = input.val().replace(/\\/g, '/').replace(/.*\//, '');
        input.trigger('fileselect', label);
    });

    $(function() {
        $(':file').on('fileselect', function(event, label) {
            var input = $(this).parents('.input-group').find(':text');
            if(this.files[0].size > 3000000){
                input.val("Please select a file under 3Mb");
                event.preventDefault();
                uploadBtn.prop('disabled', true);
                return;
            }
            if( input.length ) {
                input.val(label);
            } else {
                if(label) alert(label);
            }
            uploadBtn.prop('disabled', false);
        });
        uploadForm.submit(function(e) {
            waiter.start();
            var source = uploadSource.find(":selected").val();
            e.preventDefault();
            var fd = new FormData(uploadForm[0]);
            fd.set("source", source);
            $.post({
                url: 'recognizeUploaded',
                data: fd,
                enctype: 'multipart/form-data',
                processData: false,
                contentType: false,
                cache: false,
            }).done(function(data) {
                if(data == "you"){
                    data = messageJson[lang][data];
                    outputText.attr('data-translate', "you");
                }
                outputText.html(data);
                waiter.stop();
            });
        });
    });
});