void DCTStream::init()
{
    jpeg_std_error(&err.pub);
    err.pub.error_exit = &exitErrorHandler;
    src.pub.init_source = str_init_source;
    src.pub.fill_input_buffer = str_fill_input_buffer;
    src.pub.skip_input_data = str_skip_input_data;
    src.pub.resync_to_restart = jpeg_resync_to_restart;
    src.pub.term_source = str_term_source;
    src.pub.bytes_in_buffer = 0;
    src.pub.next_input_byte = nullptr;
    src.str = str;
    src.index = 0;
    current = nullptr;
    limit = nullptr;

    cinfo.err = &err.pub;
    if (!setjmp(err.setjmp_buffer)) {
        jpeg_create_decompress(&cinfo);
        cinfo.src = (jpeg_source_mgr *)&src;
    }
    row_buffer = nullptr;
}