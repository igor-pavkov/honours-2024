static int mov_write_avid_tag(AVIOContext *pb, MOVTrack *track)
{
    int i;
    int interlaced;
    int cid;
    int display_width = track->par->width;

    if (track->vos_data && track->vos_len > 0x29) {
        if (ff_dnxhd_parse_header_prefix(track->vos_data) != 0) {
            /* looks like a DNxHD bit stream */
            interlaced = (track->vos_data[5] & 2);
            cid = AV_RB32(track->vos_data + 0x28);
        } else {
            av_log(NULL, AV_LOG_WARNING, "Could not locate DNxHD bit stream in vos_data\n");
            return 0;
        }
    } else {
        av_log(NULL, AV_LOG_WARNING, "Could not locate DNxHD bit stream, vos_data too small\n");
        return 0;
    }

    avio_wb32(pb, 24); /* size */
    ffio_wfourcc(pb, "ACLR");
    ffio_wfourcc(pb, "ACLR");
    ffio_wfourcc(pb, "0001");
    if (track->par->color_range == AVCOL_RANGE_MPEG || /* Legal range (16-235) */
        track->par->color_range == AVCOL_RANGE_UNSPECIFIED) {
        avio_wb32(pb, 1); /* Corresponds to 709 in official encoder */
    } else { /* Full range (0-255) */
        avio_wb32(pb, 2); /* Corresponds to RGB in official encoder */
    }
    avio_wb32(pb, 0); /* unknown */

    if (track->tag == MKTAG('A','V','d','h')) {
        avio_wb32(pb, 32);
        ffio_wfourcc(pb, "ADHR");
        ffio_wfourcc(pb, "0001");
        avio_wb32(pb, cid);
        avio_wb32(pb, 0); /* unknown */
        avio_wb32(pb, 1); /* unknown */
        avio_wb32(pb, 0); /* unknown */
        avio_wb32(pb, 0); /* unknown */
        return 0;
    }

    avio_wb32(pb, 24); /* size */
    ffio_wfourcc(pb, "APRG");
    ffio_wfourcc(pb, "APRG");
    ffio_wfourcc(pb, "0001");
    avio_wb32(pb, 1); /* unknown */
    avio_wb32(pb, 0); /* unknown */

    avio_wb32(pb, 120); /* size */
    ffio_wfourcc(pb, "ARES");
    ffio_wfourcc(pb, "ARES");
    ffio_wfourcc(pb, "0001");
    avio_wb32(pb, cid); /* dnxhd cid, some id ? */
    if (   track->par->sample_aspect_ratio.num > 0
        && track->par->sample_aspect_ratio.den > 0)
        display_width = display_width * track->par->sample_aspect_ratio.num / track->par->sample_aspect_ratio.den;
    avio_wb32(pb, display_width);
    /* values below are based on samples created with quicktime and avid codecs */
    if (interlaced) {
        avio_wb32(pb, track->par->height / 2);
        avio_wb32(pb, 2); /* unknown */
        avio_wb32(pb, 0); /* unknown */
        avio_wb32(pb, 4); /* unknown */
    } else {
        avio_wb32(pb, track->par->height);
        avio_wb32(pb, 1); /* unknown */
        avio_wb32(pb, 0); /* unknown */
        if (track->par->height == 1080)
            avio_wb32(pb, 5); /* unknown */
        else
            avio_wb32(pb, 6); /* unknown */
    }
    /* padding */
    for (i = 0; i < 10; i++)
        avio_wb64(pb, 0);

    return 0;
}