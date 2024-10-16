static int cbs_jpeg_write_segment(CodedBitstreamContext *ctx,
                                  CodedBitstreamUnit *unit,
                                  PutBitContext *pbc)
{
    int err;

    if (unit->type >= JPEG_MARKER_SOF0 &&
        unit->type <= JPEG_MARKER_SOF3) {
        err = cbs_jpeg_write_frame_header(ctx, pbc, unit->content);
    } else if (unit->type >= JPEG_MARKER_APPN &&
               unit->type <= JPEG_MARKER_APPN + 15) {
        err = cbs_jpeg_write_application_data(ctx, pbc, unit->content);
    } else {
        switch (unit->type) {
#define SEGMENT(marker, func) \
            case JPEG_MARKER_ ## marker: \
                err = cbs_jpeg_write_ ## func(ctx, pbc, unit->content); \
                break;
            SEGMENT(DQT, dqt);
            SEGMENT(DHT, dht);
            SEGMENT(COM, comment);
        default:
            return AVERROR_PATCHWELCOME;
        }
    }

    return err;
}