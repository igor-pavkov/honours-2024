static int mov_write_hdlr_tag(AVFormatContext *s, AVIOContext *pb, MOVTrack *track)
{
    MOVMuxContext *mov = s->priv_data;
    const char *hdlr, *descr = NULL, *hdlr_type = NULL;
    int64_t pos = avio_tell(pb);

    hdlr      = "dhlr";
    hdlr_type = "url ";
    descr     = "DataHandler";

    if (track) {
        hdlr = (track->mode == MODE_MOV) ? "mhlr" : "\0\0\0\0";
        if (track->par->codec_type == AVMEDIA_TYPE_VIDEO) {
            hdlr_type = "vide";
            descr     = "VideoHandler";
        } else if (track->par->codec_type == AVMEDIA_TYPE_AUDIO) {
            hdlr_type = "soun";
            descr     = "SoundHandler";
        } else if (track->par->codec_type == AVMEDIA_TYPE_SUBTITLE) {
            if (is_clcp_track(track)) {
                hdlr_type = "clcp";
                descr = "ClosedCaptionHandler";
            } else {
                if (track->tag == MKTAG('t','x','3','g')) {
                    hdlr_type = "sbtl";
                } else if (track->tag == MKTAG('m','p','4','s')) {
                    hdlr_type = "subp";
                } else {
                    hdlr_type = "text";
                }
            descr = "SubtitleHandler";
            }
        } else if (track->par->codec_tag == MKTAG('r','t','p',' ')) {
            hdlr_type = "hint";
            descr     = "HintHandler";
        } else if (track->par->codec_tag == MKTAG('t','m','c','d')) {
            hdlr_type = "tmcd";
            descr = "TimeCodeHandler";
        } else if (track->par->codec_tag == MKTAG('g','p','m','d')) {
            hdlr_type = "meta";
            descr = "GoPro MET"; // GoPro Metadata
        } else {
            av_log(s, AV_LOG_WARNING,
                   "Unknown hldr_type for %s, writing dummy values\n",
                   av_fourcc2str(track->par->codec_tag));
        }
        if (track->st) {
            // hdlr.name is used by some players to identify the content title
            // of the track. So if an alternate handler description is
            // specified, use it.
            AVDictionaryEntry *t;
            t = av_dict_get(track->st->metadata, "handler_name", NULL, 0);
            if (t && utf8len(t->value))
                descr = t->value;
        }
    }

    if (mov->empty_hdlr_name) /* expressly allowed by QTFF and not prohibited in ISO 14496-12 8.4.3.3 */
        descr = "";

    avio_wb32(pb, 0); /* size */
    ffio_wfourcc(pb, "hdlr");
    avio_wb32(pb, 0); /* Version & flags */
    avio_write(pb, hdlr, 4); /* handler */
    ffio_wfourcc(pb, hdlr_type); /* handler type */
    avio_wb32(pb, 0); /* reserved */
    avio_wb32(pb, 0); /* reserved */
    avio_wb32(pb, 0); /* reserved */
    if (!track || track->mode == MODE_MOV)
        avio_w8(pb, strlen(descr)); /* pascal string */
    avio_write(pb, descr, strlen(descr)); /* handler description */
    if (track && track->mode != MODE_MOV)
        avio_w8(pb, 0); /* c string */
    return update_size(pb, pos);
}