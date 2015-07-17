(function ($) {

    Drupal.behaviors.ams_uploader = {
        attach: function (context) {
            $('input#edit-field-media-url-und-0-value').change(function() {
                if( $('input#edit-field-media-url-und-0-value').val().length > 0 && $('input#edit-field-media-url-und-0-value').val().indexOf('http') == 0){
                    $('#edit-field-authenticated-und').attr('checked', false);
                    $('#edit-field-authenticated-und').attr('disabled', true);
                }
                else {
                    $('#edit-field-authenticated-und').removeAttr("disabled");
                }
            });
        }
    }

})(jQuery);