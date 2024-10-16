static int mov_read_meta(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    while (atom.size > 8) {
        uint32_t tag;
        if (avio_feof(pb))
            return AVERROR_EOF;
        tag = avio_rl32(pb);
        atom.size -= 4;
        if (tag == MKTAG('h','d','l','r')) {
            int ret;
            avio_seek(pb, -8, SEEK_CUR);
            atom.size += 8;
            if ((ret = mov_read_default(c, pb, atom)) < 0)
                return ret;
            if (c->is_still_picture_avif) {
                int ret;
                // Add a stream for the YUV planes (primary item).
                if ((ret = avif_add_stream(c, c->primary_item_id)) < 0)
                    return ret;
                // For still AVIF images, the meta box contains all the
                // necessary information that would generally be provided by the
                // moov box. So simply mark that we have found the moov box so
                // that parsing can continue.
                c->found_moov = 1;
            }
            return ret;
        }
    }
    return 0;
}