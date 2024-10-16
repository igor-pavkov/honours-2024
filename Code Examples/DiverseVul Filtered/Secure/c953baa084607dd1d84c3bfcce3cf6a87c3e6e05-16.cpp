static int mov_read_mfra(MOVContext *c, AVIOContext *f)
{
    int64_t stream_size = avio_size(f);
    int64_t original_pos = avio_tell(f);
    int64_t seek_ret;
    int ret = -1;
    if ((seek_ret = avio_seek(f, stream_size - 4, SEEK_SET)) < 0) {
        ret = seek_ret;
        goto fail;
    }
    c->mfra_size = avio_rb32(f);
    c->have_read_mfra_size = 1;
    if (!c->mfra_size || c->mfra_size > stream_size) {
        av_log(c->fc, AV_LOG_DEBUG, "doesn't look like mfra (unreasonable size)\n");
        goto fail;
    }
    if ((seek_ret = avio_seek(f, -((int64_t) c->mfra_size), SEEK_CUR)) < 0) {
        ret = seek_ret;
        goto fail;
    }
    if (avio_rb32(f) != c->mfra_size) {
        av_log(c->fc, AV_LOG_DEBUG, "doesn't look like mfra (size mismatch)\n");
        goto fail;
    }
    if (avio_rb32(f) != MKBETAG('m', 'f', 'r', 'a')) {
        av_log(c->fc, AV_LOG_DEBUG, "doesn't look like mfra (tag mismatch)\n");
        goto fail;
    }
    av_log(c->fc, AV_LOG_VERBOSE, "stream has mfra\n");
    do {
        ret = read_tfra(c, f);
        if (ret < 0)
            goto fail;
    } while (!ret);
    ret = 0;
    c->frag_index.complete = 1;
fail:
    seek_ret = avio_seek(f, original_pos, SEEK_SET);
    if (seek_ret < 0) {
        av_log(c->fc, AV_LOG_ERROR,
               "failed to seek back after looking for mfra\n");
        ret = seek_ret;
    }
    return ret;
}