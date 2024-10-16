static bool ok_seek(ok_png_decoder *decoder, long length) {
    if (decoder->input.seek(decoder->input_user_data, length)) {
        return true;
    } else {
        ok_png_error(decoder->png, OK_PNG_ERROR_IO, "Seek error: error calling input function.");
        return false;
    }
}