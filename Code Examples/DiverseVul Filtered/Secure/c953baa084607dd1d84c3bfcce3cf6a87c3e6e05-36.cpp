static int mov_probe(const AVProbeData *p)
{
    int64_t offset;
    uint32_t tag;
    int score = 0;
    int moov_offset = -1;

    /* check file header */
    offset = 0;
    for (;;) {
        int64_t size;
        int minsize = 8;
        /* ignore invalid offset */
        if ((offset + 8ULL) > (unsigned int)p->buf_size)
            break;
        size = AV_RB32(p->buf + offset);
        if (size == 1 && offset + 16 <= (unsigned int)p->buf_size) {
            size = AV_RB64(p->buf+offset + 8);
            minsize = 16;
        } else if (size == 0) {
            size = p->buf_size - offset;
        }
        if (size < minsize) {
            offset += 4;
            continue;
        }
        tag = AV_RL32(p->buf + offset + 4);
        switch(tag) {
        /* check for obvious tags */
        case MKTAG('m','o','o','v'):
            moov_offset = offset + 4;
        case MKTAG('m','d','a','t'):
        case MKTAG('p','n','o','t'): /* detect movs with preview pics like ew.mov and april.mov */
        case MKTAG('u','d','t','a'): /* Packet Video PVAuthor adds this and a lot of more junk */
        case MKTAG('f','t','y','p'):
            if (tag == MKTAG('f','t','y','p') &&
                       (   AV_RL32(p->buf + offset + 8) == MKTAG('j','p','2',' ')
                        || AV_RL32(p->buf + offset + 8) == MKTAG('j','p','x',' ')
                        || AV_RL32(p->buf + offset + 8) == MKTAG('j','x','l',' ')
                    )) {
                score = FFMAX(score, 5);
            } else {
                score = AVPROBE_SCORE_MAX;
            }
            break;
        /* those are more common words, so rate then a bit less */
        case MKTAG('e','d','i','w'): /* xdcam files have reverted first tags */
        case MKTAG('w','i','d','e'):
        case MKTAG('f','r','e','e'):
        case MKTAG('j','u','n','k'):
        case MKTAG('p','i','c','t'):
            score  = FFMAX(score, AVPROBE_SCORE_MAX - 5);
            break;
        case MKTAG(0x82,0x82,0x7f,0x7d):
        case MKTAG('s','k','i','p'):
        case MKTAG('u','u','i','d'):
        case MKTAG('p','r','f','l'):
            /* if we only find those cause probedata is too small at least rate them */
            score  = FFMAX(score, AVPROBE_SCORE_EXTENSION);
            break;
        }
        if (size > INT64_MAX - offset)
            break;
        offset += size;
    }
    if (score > AVPROBE_SCORE_MAX - 50 && moov_offset != -1) {
        /* moov atom in the header - we should make sure that this is not a
         * MOV-packed MPEG-PS */
        offset = moov_offset;

        while (offset < (p->buf_size - 16)) { /* Sufficient space */
               /* We found an actual hdlr atom */
            if (AV_RL32(p->buf + offset     ) == MKTAG('h','d','l','r') &&
                AV_RL32(p->buf + offset +  8) == MKTAG('m','h','l','r') &&
                AV_RL32(p->buf + offset + 12) == MKTAG('M','P','E','G')) {
                av_log(NULL, AV_LOG_WARNING, "Found media data tag MPEG indicating this is a MOV-packed MPEG-PS.\n");
                /* We found a media handler reference atom describing an
                 * MPEG-PS-in-MOV, return a
                 * low score to force expanding the probe window until
                 * mpegps_probe finds what it needs */
                return 5;
            } else {
                /* Keep looking */
                offset += 2;
            }
        }
    }

    return score;
}