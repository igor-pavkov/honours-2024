static void ok_png_set_error(ok_png *png, ok_png_error error_code) {
    if (png) {
        png->width = 0;
        png->height = 0;
        png->error_code = error_code;
    }
}