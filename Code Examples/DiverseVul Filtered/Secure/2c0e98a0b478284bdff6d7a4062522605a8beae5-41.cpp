static int mov_create_chapter_track(AVFormatContext *s, int tracknum)
{
    AVIOContext *pb;

    MOVMuxContext *mov = s->priv_data;
    MOVTrack *track = &mov->tracks[tracknum];
    AVPacket pkt = { .stream_index = tracknum, .flags = AV_PKT_FLAG_KEY };
    int i, len;

    track->mode = mov->mode;
    track->tag = MKTAG('t','e','x','t');
    track->timescale = MOV_TIMESCALE;
    track->par = avcodec_parameters_alloc();
    if (!track->par)
        return AVERROR(ENOMEM);
    track->par->codec_type = AVMEDIA_TYPE_SUBTITLE;
#if 0
    // These properties are required to make QT recognize the chapter track
    uint8_t chapter_properties[43] = { 0, 0, 0, 0, 0, 0, 0, 1, };
    if (ff_alloc_extradata(track->par, sizeof(chapter_properties)))
        return AVERROR(ENOMEM);
    memcpy(track->par->extradata, chapter_properties, sizeof(chapter_properties));
#else
    if (avio_open_dyn_buf(&pb) >= 0) {
        int size;
        uint8_t *buf;

        /* Stub header (usually for Quicktime chapter track) */
        // TextSampleEntry
        avio_wb32(pb, 0x01); // displayFlags
        avio_w8(pb, 0x00);   // horizontal justification
        avio_w8(pb, 0x00);   // vertical justification
        avio_w8(pb, 0x00);   // bgColourRed
        avio_w8(pb, 0x00);   // bgColourGreen
        avio_w8(pb, 0x00);   // bgColourBlue
        avio_w8(pb, 0x00);   // bgColourAlpha
        // BoxRecord
        avio_wb16(pb, 0x00); // defTextBoxTop
        avio_wb16(pb, 0x00); // defTextBoxLeft
        avio_wb16(pb, 0x00); // defTextBoxBottom
        avio_wb16(pb, 0x00); // defTextBoxRight
        // StyleRecord
        avio_wb16(pb, 0x00); // startChar
        avio_wb16(pb, 0x00); // endChar
        avio_wb16(pb, 0x01); // fontID
        avio_w8(pb, 0x00);   // fontStyleFlags
        avio_w8(pb, 0x00);   // fontSize
        avio_w8(pb, 0x00);   // fgColourRed
        avio_w8(pb, 0x00);   // fgColourGreen
        avio_w8(pb, 0x00);   // fgColourBlue
        avio_w8(pb, 0x00);   // fgColourAlpha
        // FontTableBox
        avio_wb32(pb, 0x0D); // box size
        ffio_wfourcc(pb, "ftab"); // box atom name
        avio_wb16(pb, 0x01); // entry count
        // FontRecord
        avio_wb16(pb, 0x01); // font ID
        avio_w8(pb, 0x00);   // font name length

        if ((size = avio_close_dyn_buf(pb, &buf)) > 0) {
            track->par->extradata = buf;
            track->par->extradata_size = size;
        } else {
            av_freep(&buf);
        }
    }
#endif

    for (i = 0; i < s->nb_chapters; i++) {
        AVChapter *c = s->chapters[i];
        AVDictionaryEntry *t;

        int64_t end = av_rescale_q(c->end, c->time_base, (AVRational){1,MOV_TIMESCALE});
        pkt.pts = pkt.dts = av_rescale_q(c->start, c->time_base, (AVRational){1,MOV_TIMESCALE});
        pkt.duration = end - pkt.dts;

        if ((t = av_dict_get(c->metadata, "title", NULL, 0))) {
            static const char encd[12] = {
                0x00, 0x00, 0x00, 0x0C,
                'e',  'n',  'c',  'd',
                0x00, 0x00, 0x01, 0x00 };
            len      = strlen(t->value);
            pkt.size = len + 2 + 12;
            pkt.data = av_malloc(pkt.size);
            if (!pkt.data)
                return AVERROR(ENOMEM);
            AV_WB16(pkt.data, len);
            memcpy(pkt.data + 2, t->value, len);
            memcpy(pkt.data + len + 2, encd, sizeof(encd));
            ff_mov_write_packet(s, &pkt);
            av_freep(&pkt.data);
        }
    }

    return 0;
}