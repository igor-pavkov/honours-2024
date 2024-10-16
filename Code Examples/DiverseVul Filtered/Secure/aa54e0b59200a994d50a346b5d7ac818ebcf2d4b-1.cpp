static void shape_harfbuzz(ASS_Shaper *shaper, GlyphInfo *glyphs, size_t len)
{
    int i;
    hb_buffer_t *buf = hb_buffer_create();
    hb_segment_properties_t props = HB_SEGMENT_PROPERTIES_DEFAULT;

    // Initialize: skip all glyphs, this is undone later as needed
    for (i = 0; i < len; i++)
        glyphs[i].skip = 1;

    for (i = 0; i < len; i++) {
        int offset = i;
        hb_font_t *font = get_hb_font(shaper, glyphs + offset);
        int level = glyphs[offset].shape_run_id;
        int direction = shaper->emblevels[offset] % 2;

        // advance in text until end of run
        while (i < (len - 1) && level == glyphs[i+1].shape_run_id)
            i++;

        hb_buffer_pre_allocate(buf, i - offset + 1);
        hb_buffer_add_utf32(buf, shaper->event_text + offset, i - offset + 1,
                0, i - offset + 1);

        props.direction = direction ? HB_DIRECTION_RTL : HB_DIRECTION_LTR;
        props.script = glyphs[offset].script;
        props.language  = hb_shaper_get_run_language(shaper, props.script);
        hb_buffer_set_segment_properties(buf, &props);

        set_run_features(shaper, glyphs + offset);
        hb_shape(font, buf, shaper->features, shaper->n_features);

        shape_harfbuzz_process_run(glyphs, buf, offset);
        hb_buffer_reset(buf);
    }

    hb_buffer_destroy(buf);
}