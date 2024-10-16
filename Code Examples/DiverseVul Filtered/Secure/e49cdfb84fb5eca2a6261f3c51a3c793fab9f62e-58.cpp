static bool ok_read(ok_png_decoder *decoder, uint8_t *buffer, size_t length) {
    if (decoder->input.read(decoder->input_user_data, buffer, length) == length) {
        return true;
    } else {
        ok_png_error(decoder->png, OK_PNG_ERROR_IO, "Read error: error calling input function.");
        return false;
    }
}