static int mov_write_eac3_tag(AVIOContext *pb, MOVTrack *track)
{
    PutBitContext pbc;
    uint8_t *buf;
    struct eac3_info *info;
    int size, i;

    if (!track->eac3_priv)
        return AVERROR(EINVAL);

    info = track->eac3_priv;
    size = 2 + 4 * (info->num_ind_sub + 1);
    buf = av_malloc(size);
    if (!buf) {
        size = AVERROR(ENOMEM);
        goto end;
    }

    init_put_bits(&pbc, buf, size);
    put_bits(&pbc, 13, info->data_rate);
    put_bits(&pbc,  3, info->num_ind_sub);
    for (i = 0; i <= info->num_ind_sub; i++) {
        put_bits(&pbc, 2, info->substream[i].fscod);
        put_bits(&pbc, 5, info->substream[i].bsid);
        put_bits(&pbc, 1, 0); /* reserved */
        put_bits(&pbc, 1, 0); /* asvc */
        put_bits(&pbc, 3, info->substream[i].bsmod);
        put_bits(&pbc, 3, info->substream[i].acmod);
        put_bits(&pbc, 1, info->substream[i].lfeon);
        put_bits(&pbc, 5, 0); /* reserved */
        put_bits(&pbc, 4, info->substream[i].num_dep_sub);
        if (!info->substream[i].num_dep_sub) {
            put_bits(&pbc, 1, 0); /* reserved */
            size--;
        } else {
            put_bits(&pbc, 9, info->substream[i].chan_loc);
        }
    }
    flush_put_bits(&pbc);

    avio_wb32(pb, size + 8);
    ffio_wfourcc(pb, "dec3");
    avio_write(pb, buf, size);

    av_free(buf);

end:
    av_packet_unref(&info->pkt);
    av_freep(&track->eac3_priv);

    return size;
}