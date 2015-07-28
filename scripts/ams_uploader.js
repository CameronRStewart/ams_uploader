(function ($) {

    Drupal.behaviors.ams_uploader = {
        attach: function (context) {

            /* No Longer Neccesary
            $('input#edit-field-media-url-und-0-value').change(function() {
                if( $('input#edit-field-media-url-und-0-value').val().length > 0 && $('input#edit-field-media-url-und-0-value').val().indexOf('http') === 0){
                    $('#edit-field-authenticated-und').attr('checked', false);
                    $('#edit-field-authenticated-und').attr('disabled', true);
                }
                else {
                    $('#edit-field-authenticated-und').removeAttr("disabled");
                }
            });
            */


            setInterval(function () { track_fid_change()}, 1000);

        }
    }

    function track_fid_change() {
        if($('input[name="field_media_upload[und][0][fid]"]').val() == 0) {
            $("input#edit-field-media-url-und-0-value").removeAttr('disabled');
        }
        else {
            $("input#edit-field-media-url-und-0-value").attr('disabled', true);
        }
    };


})(jQuery);