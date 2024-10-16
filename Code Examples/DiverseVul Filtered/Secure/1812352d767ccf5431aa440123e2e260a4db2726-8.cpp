static int cbs_jpeg_write_scan(CodedBitstreamContext *ctx,
                               CodedBitstreamUnit *unit,
                               PutBitContext *pbc)
{
    JPEGRawScan *scan = unit->content;
    int err;

    err = cbs_jpeg_write_scan_header(ctx, pbc, &scan->header);
    if (err < 0)
        return err;

    if (scan->data) {
        if (scan->data_size * 8 > put_bits_left(pbc))
            return AVERROR(ENOSPC);

        av_assert0(put_bits_count(pbc) % 8 == 0);

        flush_put_bits(pbc);

        memcpy(put_bits_ptr(pbc), scan->data, scan->data_size);
        skip_put_bytes(pbc, scan->data_size);
    }

    return 0;
}