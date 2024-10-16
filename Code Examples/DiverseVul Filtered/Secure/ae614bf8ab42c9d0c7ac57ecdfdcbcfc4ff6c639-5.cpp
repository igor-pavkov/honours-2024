void DCTStream::reset()
{
    int row_stride;

    str->reset();

    if (row_buffer) {
        jpeg_destroy_decompress(&cinfo);
        init();
    }

    // JPEG data has to start with 0xFF 0xD8
    // but some pdf like the one on
    // https://bugs.freedesktop.org/show_bug.cgi?id=3299
    // does have some garbage before that this seeks for
    // the start marker...
    bool startFound = false;
    int c = 0, c2 = 0;
    while (!startFound) {
        if (!c) {
            c = str->getChar();
            if (c == -1) {
                error(errSyntaxError, -1, "Could not find start of jpeg data");
                return;
            }
            if (c != 0xFF)
                c = 0;
        } else {
            c2 = str->getChar();
            if (c2 != 0xD8) {
                c = 0;
                c2 = 0;
            } else
                startFound = true;
        }
    }

    if (!setjmp(err.setjmp_buffer)) {
        if (jpeg_read_header(&cinfo, TRUE) != JPEG_SUSPENDED) {
            // figure out color transform
            if (colorXform == -1 && !cinfo.saw_Adobe_marker) {
                if (cinfo.num_components == 3) {
                    if (cinfo.saw_JFIF_marker) {
                        colorXform = 1;
                    } else if (cinfo.cur_comp_info[0]->component_id == 82 && cinfo.cur_comp_info[1]->component_id == 71 && cinfo.cur_comp_info[2]->component_id == 66) { // ASCII "RGB"
                        colorXform = 0;
                    } else {
                        colorXform = 1;
                    }
                } else {
                    colorXform = 0;
                }
            } else if (cinfo.saw_Adobe_marker) {
                colorXform = cinfo.Adobe_transform;
            }

            switch (cinfo.num_components) {
            case 3:
                cinfo.jpeg_color_space = colorXform ? JCS_YCbCr : JCS_RGB;
                break;
            case 4:
                cinfo.jpeg_color_space = colorXform ? JCS_YCCK : JCS_CMYK;
                break;
            }

            jpeg_start_decompress(&cinfo);

            row_stride = cinfo.output_width * cinfo.output_components;
            row_buffer = cinfo.mem->alloc_sarray((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);
        }
    }
}