static int cbs_jpeg_read_unit(CodedBitstreamContext *ctx,
                              CodedBitstreamUnit *unit)
{
    GetBitContext gbc;
    int err;

    err = init_get_bits(&gbc, unit->data, 8 * unit->data_size);
    if (err < 0)
        return err;

    if (unit->type >= JPEG_MARKER_SOF0 &&
        unit->type <= JPEG_MARKER_SOF3) {
        err = ff_cbs_alloc_unit_content(ctx, unit,
                                        sizeof(JPEGRawFrameHeader),
                                        NULL);
        if (err < 0)
            return err;

        err = cbs_jpeg_read_frame_header(ctx, &gbc, unit->content);
        if (err < 0)
            return err;

    } else if (unit->type >= JPEG_MARKER_APPN &&
               unit->type <= JPEG_MARKER_APPN + 15) {
        err = ff_cbs_alloc_unit_content(ctx, unit,
                                        sizeof(JPEGRawApplicationData),
                                        &cbs_jpeg_free_application_data);
        if (err < 0)
            return err;

        err = cbs_jpeg_read_application_data(ctx, &gbc, unit->content);
        if (err < 0)
            return err;

    } else if (unit->type == JPEG_MARKER_SOS) {
        JPEGRawScan *scan;
        int pos;

        err = ff_cbs_alloc_unit_content(ctx, unit,
                                        sizeof(JPEGRawScan),
                                        &cbs_jpeg_free_scan);
        if (err < 0)
            return err;
        scan = unit->content;

        err = cbs_jpeg_read_scan_header(ctx, &gbc, &scan->header);
        if (err < 0)
            return err;

        pos = get_bits_count(&gbc);
        av_assert0(pos % 8 == 0);
        if (pos > 0) {
            scan->data_size = unit->data_size - pos / 8;
            scan->data_ref  = av_buffer_ref(unit->data_ref);
            if (!scan->data_ref)
                return AVERROR(ENOMEM);
            scan->data = unit->data + pos / 8;
        }

    } else {
        switch (unit->type) {
#define SEGMENT(marker, type, func, free) \
        case JPEG_MARKER_ ## marker: \
            { \
                err = ff_cbs_alloc_unit_content(ctx, unit, \
                                                sizeof(type), free); \
                if (err < 0) \
                    return err; \
                err = cbs_jpeg_read_ ## func(ctx, &gbc, unit->content); \
                if (err < 0) \
                    return err; \
            } \
            break
            SEGMENT(DQT, JPEGRawQuantisationTableSpecification, dqt, NULL);
            SEGMENT(DHT, JPEGRawHuffmanTableSpecification,      dht, NULL);
            SEGMENT(COM, JPEGRawComment,  comment, &cbs_jpeg_free_comment);
#undef SEGMENT
        default:
            return AVERROR(ENOSYS);
        }
    }

    return 0;
}