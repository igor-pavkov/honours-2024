static int mov_read_moof(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    // Set by mov_read_tfhd(). mov_read_trun() will reject files missing tfhd.
    c->fragment.found_tfhd = 0;

    if (!c->has_looked_for_mfra && c->use_mfra_for > 0) {
        c->has_looked_for_mfra = 1;
        if (pb->seekable & AVIO_SEEKABLE_NORMAL) {
            int ret;
            av_log(c->fc, AV_LOG_VERBOSE, "stream has moof boxes, will look "
                    "for a mfra\n");
            if ((ret = mov_read_mfra(c, pb)) < 0) {
                av_log(c->fc, AV_LOG_VERBOSE, "found a moof box but failed to "
                        "read the mfra (may be a live ismv)\n");
            }
        } else {
            av_log(c->fc, AV_LOG_VERBOSE, "found a moof box but stream is not "
                    "seekable, can not look for mfra\n");
        }
    }
    c->fragment.moof_offset = c->fragment.implicit_offset = avio_tell(pb) - 8;
    av_log(c->fc, AV_LOG_TRACE, "moof offset %"PRIx64"\n", c->fragment.moof_offset);
    c->frag_index.current = update_frag_index(c, c->fragment.moof_offset);
    return mov_read_default(c, pb, atom);
}