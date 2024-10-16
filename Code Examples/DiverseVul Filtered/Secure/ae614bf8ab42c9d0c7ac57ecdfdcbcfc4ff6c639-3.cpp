static void exitErrorHandler(jpeg_common_struct *error)
{
    j_decompress_ptr cinfo = (j_decompress_ptr)error;
    str_error_mgr *err = (struct str_error_mgr *)cinfo->err;
    if (cinfo->err->msg_code == JERR_IMAGE_TOO_BIG && err->width != 0 && err->height != 0) {
        cinfo->image_height = err->height;
        cinfo->image_width = err->width;
    } else {
        longjmp(err->setjmp_buffer, 1);
    }
}