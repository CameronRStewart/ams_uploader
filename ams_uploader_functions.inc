<?php
/**
 * @file
 * A description of what your module does.
 */


function is_http($media_url) {
    return (strpos($media_url, 'http') === 0);
}

function is_rtmp($media_url) {
    return (strpos($media_url, 'rtmp') === 0);
}

function remove_str_between($delimiter1, $delimiter2, $uri) {

    /**
     * This function will take a string like rtmp://libmedia.unm.edu/vod/mp4:test3.f4v,
     * remove the mp4: part and return the rest: rtmp://libmedia.unm.edu/vod/test3.f4v
     */

    $slice_front = strpos($uri, $delimiter1);
    $slice_back = strpos($uri, $delimiter2);

    if($slice_front === false || $slice_back === false) {
        return $uri;
    }
    else {
        $remove_str = substr($uri, $slice_front, ($slice_back + strlen($delimiter2)) - $slice_front);
        return str_replace($remove_str, '/', $uri);
    }
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

    $parsed_url_path = explode('/', $media_url, 5);
    $application = $parsed_url_path[3];
    $path_tail = $parsed_url_path[4];

    // Remove AMS required filetype prefixes.
    //$path_tail = remove_str_between('/', ':', $path_tail);
    $path_tail = strip_filetype_prefix($path_tail, ':');

    if (is_http($media_url)) {
        $local_path = 'ams://webroot/' . $application . '/' . $path_tail;
    }
    else if (is_rtmp($media_url)) {
        $local_path = 'ams://applications/' . $application . '/media/' . $path_tail;
        // appdir/media/subdir
    }
    else {
        return false;
    }

    if(file_exists($local_path)) {
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
    $newpath = $appDir . '/' . $subDir;

    return $newpath;



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

function generate_url($basename, $ext, $subDir, $appName) {
    //Grab the FQDN of the media stremaing server
    $result = db_query('SELECT * FROM ams_uploader_admin_vars');

    foreach ($result as $row) {
        $fqdn = $row->name;
    }


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