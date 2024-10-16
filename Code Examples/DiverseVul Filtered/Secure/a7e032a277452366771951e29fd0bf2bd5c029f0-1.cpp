static void rm_read_metadata(AVFormatContext *s, AVIOContext *pb, int wide)
{
    char buf[1024];
    int i;

    for (i=0; i<FF_ARRAY_ELEMS(ff_rm_metadata); i++) {
        int len = wide ? avio_rb16(pb) : avio_r8(pb);
        if (len > 0) {
            get_strl(pb, buf, sizeof(buf), len);
            av_dict_set(&s->metadata, ff_rm_metadata[i], buf, 0);
        }
    }
}