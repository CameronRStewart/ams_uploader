<?php
/**
 * @file
 * A description of what your module does.
 */

function generate_url($basename, $ext, $subDir, $appName) {

    //TODO: change function signiture to have a server var.  If that var isnt passed in, then we use fqdn.

    $fqdn = get_user_configured_hostname();


    //Generate rtmp url for file
    if(!(empty($subDir)) || !($subDir == '')){
        $subDir = $subDir . '/';
    }

    $url = 'rtmp://' . $fqdn . '/' . $appName . '/' . $subDir;
    //Use extension to determine how to build the rtmp url correctly
    if ($ext == 'mp4' || $ext == 'f4v') {
        $amsExt = 'mp4' . ':';
    }
    else {
        if ($ext == 'flv') {
            $amsExt = '';
            $ext = '';
        }
        else {
            $amsExt = $ext . ':';
        }
    }
    //Add dot to extension if it exists
    if (!empty($ext)) {
        $ext = '.' . $ext;
    }

    $new_url = $url . $amsExt . $basename . $ext;

    return $new_url;
}

function get_app_name_and_path_tail_from_url($url) {
    $parsed_url_path = explode('/', $url, 5);
    $app_name = $parsed_url_path[3];
    $path_tail = $parsed_url_path[4];

    $ret = array($app_name, $path_tail);

    return $ret;
}

function get_filename_and_extension($uri) {
    $ext = pathinfo($uri, PATHINFO_EXTENSION);
    $filename_no_ext = pathinfo($uri, PATHINFO_FILENAME);

    //Use extension to determine how to build the rtmp url correctly
    if ($ext == '' || empty($ext)) {
        $ext = "flv";
    }

    $split_name = array($filename_no_ext, $ext);

    return $split_name;

}

function get_user_configured_hostname() {

    //Grab the FQDN of the media stremaing server
    $result = db_query('SELECT * FROM ams_uploader_admin_vars');

    $count = $result->rowCount();

    $fqdn = '';

    if(isset($result) && !(empty($result)) && $count == 1) {
        foreach ($result as $row) {
            $fqdn = $row->name;
        }
    }
    else {
        throw new Exception("get_hostname: FQDN query yeilded " . $count . " values, expected 1.");
    }

    return $fqdn;

}

function get_local_path_or_false($media_url) {

    /**
     * When a URL is passed in for saving, we need to check whether that URL can be
     * parsed and if it points to something on the designated streaming server for this install.
     * This function parses a url, and tries to load it to make sure this file is both local and exists.
     *
     * Examples:
     *  mp4: rtmp://libmedia.unm.edu/vod/mp4:test3.f4v
     *  flv: rtmp://libmedia.unm.edu/unm/NavajoFilmThemselves/NavajoFilmThemselves_1-2
     *  flv: http://libmedia.unm.edu/vod/bingaman/A1-05/F02.flv with a bunch of options after the &...
     *
     * Paths we need from these:
     *  ams://applications/vod/media/test3.f4v
     *  ams://applications/unm/media/NavajoFilmThemselves/NavajoFilmThemselves_1-2.flv
     *  ams://webroot/vod/bingaman/A1-05/02.flv
     **/

    // TODO: deal with flv files with no extension

    //$parsed_url_path = explode('/', $media_url, 5);
    //$application = $parsed_url_path[3];
    //$path_tail = $parsed_url_path[4];

    list($application, $path_tail) = get_app_name_and_path_tail_from_url($media_url);

    // Remove AMS required filetype prefixes.
    //$path_tail = remove_str_between('/', ':', $path_tail);
    $path_tail = strip_filetype_prefix($path_tail, ':');

    if (is_http($media_url)) {
        $new_path_tail = '/' . $application . '/' . $path_tail;
        $local_path = 'ams://webroot' . $new_path_tail;
        $pre_replace_to_get_hostname_from_user_entered_url = 'http://';
        $post_replace_to_get_hostname_from_user_entered_url = $new_path_tail;
    }
    else if (is_rtmp($media_url)) {
        $local_path = 'ams://applications/' . $application . '/media/' . $path_tail;
        $pre_replace_to_get_hostname_from_user_entered_url = 'rtmp://';
        $post_replace_to_get_hostname_from_user_entered_url = '/' . $application . '/' . $path_tail;
    }
    else {
        return false;
    }

    $hostname_from_url = str_replace($pre_replace_to_get_hostname_from_user_entered_url, '', $media_url);
    $hostname_from_url = str_replace($post_replace_to_get_hostname_from_user_entered_url, '', $hostname_from_url);
    $actual_hostname = get_user_configured_hostname();
    if(file_exists($local_path) && ($hostname_from_url == $actual_hostname)) {
        return $local_path;
    }
    else {
        return false;
    }

}

function get_new_path($appName, $subDir) {
    // TODO: create seperate filesystem wrappers for RTMP and HTML urls.

    $protocolDir = 'applications';
    $appDir = 'ams://' . $protocolDir . '/' . $appName . '/media';
    if(!(empty($subDir)) || !($subDir == '')){
        $newpath = $appDir . '/' . $subDir;
    }
    else {
        $newpath = $appDir;
    }

    return $newpath;



}

function get_object_code($height, $width, $url) {
    $mod_path = drupal_get_path('module', 'ams_uploader');
    //$player = base_path() . $mod_path . '/swfs/StrobeMediaPlayback.swf';
    $results = db_query('SELECT * FROM ams_uploader_media_player');
    $count = $results->rowCount();

    if (isset($results) && !empty($results) && $count > 0) {
        foreach ($results as $row) {
            $player = $row->player_uri;
        }
    }

    $embedCode = '
    <object type="application/x-shockwave-flash" data="' . $player . '"
      width="' . $width . '" height="' . $height . '">
      <param name="quality" value="high">
      <param name="bgcolor" value="#000000">
      <param name="allowscriptaccess" value="sameDomain">
      <param name="allowfullscreen" value="true">
      <param name="wmode" value="direct">
      <param name="flashvars" value="src=' . $url . '">
    </object>
    ';

    return $embedCode;
}

function get_str_between($uri, $delimiter1, $delimiter2) {

    /**
     * This function will take a string like rtmp://libmedia.unm.edu/vod/mp4:test3.f4v,
     * remove the mp4: part and return the rest: rtmp://libmedia.unm.edu/vod/test3.f4v
     */

    $slice_front = strpos($uri, $delimiter1);
    $length_front = strlen($delimiter1);
    $slice_back = strpos($uri, $delimiter2);


    if($slice_front === false || $slice_back === false) {
        return $uri;
    }
    else {
        $index = $slice_front + $length_front;
        $length = $slice_back - $index;
        $remove_str = substr($uri, $index, $length);

        $remove_str = strip_outer_slashes($remove_str);

        return $remove_str;
    }
}

function strip_outer_slashes($string) {
    if($string) {
        // Remove '/' from front and back if they are there.
        if($string[0] == '/'){
            $string = substr($string, 1);
        }
        if($string[strlen($string) - 1] == '/'){
            $string = substr($string, 0, -1);
        }
    }
    return $string;
}

function is_http($media_url) {
    return (strpos($media_url, 'http') === 0);
}

function is_rtmp($media_url) {
    return (strpos($media_url, 'rtmp') === 0);
}

function strip_filetype_prefix($string, $delimiter) {
    $slice = strpos($string, $delimiter);

    if($slice === false){
        return $string;
    }
    else {
        $remove_str = substr($string, 0, ($slice + strlen($delimiter)));
        return str_replace($remove_str, '', $string);
    }
}

