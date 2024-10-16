static int cbs_jpeg_write_scan(CodedBitstreamContext *ctx,
                               CodedBitstreamUnit *unit,
                               PutBitContext *pbc)
{
    JPEGRawScan *scan = unit->content;
    int i, err;

    err = cbs_jpeg_write_scan_header(ctx, pbc, &scan->header);
    if (err < 0)
        return err;

    if (scan->data) {
        if (scan->data_size * 8 > put_bits_left(pbc))
            return AVERROR(ENOSPC);

        for (i = 0; i < scan->data_size; i++)
            put_bits(pbc, 8, scan->data[i]);
    }

    return 0;
}