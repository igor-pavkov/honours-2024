bool DCTStream::readLine()
{
    if (cinfo.output_scanline < cinfo.output_height) {
        if (!setjmp(err.setjmp_buffer)) {
            if (!jpeg_read_scanlines(&cinfo, row_buffer, 1))
                return false;
            else {
                current = &row_buffer[0][0];
                limit = &row_buffer[0][(cinfo.output_width - 1) * cinfo.output_components] + cinfo.output_components;
                return true;
            }
        } else
            return false;
    } else
        return false;
}