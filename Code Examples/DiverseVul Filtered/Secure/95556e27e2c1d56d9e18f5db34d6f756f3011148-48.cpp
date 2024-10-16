static int mov_write_moof_tag(AVIOContext *pb, MOVMuxContext *mov, int tracks,
                              int64_t mdat_size)
{
    AVIOContext *avio_buf;
    int ret, moof_size;

    if ((ret = ffio_open_null_buf(&avio_buf)) < 0)
        return ret;
    mov_write_moof_tag_internal(avio_buf, mov, tracks, 0);
    moof_size = ffio_close_null_buf(avio_buf);

    if (mov->flags & FF_MOV_FLAG_DASH && !(mov->flags & FF_MOV_FLAG_GLOBAL_SIDX))
        mov_write_sidx_tags(pb, mov, tracks, moof_size + 8 + mdat_size);

    if (mov->write_prft > MOV_PRFT_NONE && mov->write_prft < MOV_PRFT_NB)
        mov_write_prft_tag(pb, mov, tracks);

    if (mov->flags & FF_MOV_FLAG_GLOBAL_SIDX ||
        !(mov->flags & FF_MOV_FLAG_SKIP_TRAILER) ||
        mov->ism_lookahead) {
        if ((ret = mov_add_tfra_entries(pb, mov, tracks, moof_size + 8 + mdat_size)) < 0)
            return ret;
        if (!(mov->flags & FF_MOV_FLAG_GLOBAL_SIDX) &&
            mov->flags & FF_MOV_FLAG_SKIP_TRAILER) {
            mov_prune_frag_info(mov, tracks, mov->ism_lookahead + 1);
        }
    }

    return mov_write_moof_tag_internal(pb, mov, tracks, moof_size);
}