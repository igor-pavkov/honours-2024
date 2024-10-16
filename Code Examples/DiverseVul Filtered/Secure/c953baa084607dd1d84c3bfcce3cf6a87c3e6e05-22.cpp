static int mov_read_ares(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    if (c->fc->nb_streams >= 1) {
        AVStream *const  st = c->fc->streams[c->fc->nb_streams - 1];
        FFStream *const sti = ffstream(st);
        AVCodecParameters *par = st->codecpar;

        if (par->codec_tag == MKTAG('A', 'V', 'i', 'n') &&
            par->codec_id == AV_CODEC_ID_H264 &&
            atom.size > 11) {
            int cid;
            avio_skip(pb, 10);
            cid = avio_rb16(pb);
            /* For AVID AVCI50, force width of 1440 to be able to select the correct SPS and PPS */
            if (cid == 0xd4d || cid == 0xd4e)
                par->width = 1440;
            return 0;
        } else if ((par->codec_tag == MKTAG('A', 'V', 'd', '1') ||
                    par->codec_tag == MKTAG('A', 'V', 'j', '2') ||
                    par->codec_tag == MKTAG('A', 'V', 'd', 'n')) &&
                   atom.size >= 24) {
            int num, den;
            avio_skip(pb, 12);
            num = avio_rb32(pb);
            den = avio_rb32(pb);
            if (num <= 0 || den <= 0)
                return 0;
            switch (avio_rb32(pb)) {
            case 2:
                if (den >= INT_MAX / 2)
                    return 0;
                den *= 2;
            case 1:
                sti->display_aspect_ratio = (AVRational){ num, den };
            default:
                return 0;
            }
        }
    }

    return mov_read_avid(c, pb, atom);
}