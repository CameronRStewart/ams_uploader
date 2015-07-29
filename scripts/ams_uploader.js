(function ($) {
    // TODO: Grey-out of disabled input options is untenable in current form.  Need to apply class on change, and set css in theme to set background-color based on this class.
    Drupal.behaviors.ams_uploader = {
        attach: function (context) {

            // Upon new node creation, if a user enters a URL, disable subdirectory field.
            $('input#edit-field-media-url-und-0-value').change(function() {
                if( $('input#edit-field-media-url-und-0-value').val().length > 0 && $('input[name="field_media_upload[und][0][fid]"]').val() == 0){
                    $('input#edit-field-directory-und-0-value').attr('disabled', true);
                    $('input#edit-field-directory-und-0-value').css('background-color' , '#DEDEDE');
                }
                else {
                    $('input#edit-field-directory-und-0-value').removeAttr("disabled");
                    $('input#edit-field-directory-und-0-value').css('background-color' , '#FFFFFF');
                }
            });


            // If a file has been uploaded, disable url input field.
            setInterval(function () { track_fid_change()}, 1000);

        }
    }

    function track_fid_change() {
        if($('input[name="field_media_upload[und][0][fid]"]').val() == 0) {
            $("input#edit-field-media-url-und-0-value").removeAttr('disabled');
            $("input#edit-field-media-url-und-0-value").css('background-color' , '#FFFFFF');
        }
        else {
            $("input#edit-field-media-url-und-0-value").attr('disabled', true);
            $("input#edit-field-media-url-und-0-value").css('background-color' , '#DEDEDE');
        }
    };


})(jQuery);