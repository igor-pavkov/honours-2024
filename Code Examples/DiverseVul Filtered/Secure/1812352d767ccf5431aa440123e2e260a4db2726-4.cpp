static int cbs_jpeg_write_unit(CodedBitstreamContext *ctx,
                               CodedBitstreamUnit *unit,
                               PutBitContext *pbc)
{
    if (unit->type == JPEG_MARKER_SOS)
        return cbs_jpeg_write_scan   (ctx, unit, pbc);
    else
        return cbs_jpeg_write_segment(ctx, unit, pbc);
}