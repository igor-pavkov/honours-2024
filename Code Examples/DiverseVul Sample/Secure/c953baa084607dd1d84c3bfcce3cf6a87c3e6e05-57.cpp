static int mov_switch_root(AVFormatContext *s, int64_t target, int index)
{
    int ret;
    MOVContext *mov = s->priv_data;

    if (index >= 0 && index < mov->frag_index.nb_items)
        target = mov->frag_index.item[index].moof_offset;
    if (avio_seek(s->pb, target, SEEK_SET) != target) {
        av_log(mov->fc, AV_LOG_ERROR, "root atom offset 0x%"PRIx64": partial file\n", target);
        return AVERROR_INVALIDDATA;
    }

    mov->next_root_atom = 0;
    if (index < 0 || index >= mov->frag_index.nb_items)
        index = search_frag_moof_offset(&mov->frag_index, target);
    if (index < mov->frag_index.nb_items &&
        mov->frag_index.item[index].moof_offset == target) {
        if (index + 1 < mov->frag_index.nb_items)
            mov->next_root_atom = mov->frag_index.item[index + 1].moof_offset;
        if (mov->frag_index.item[index].headers_read)
            return 0;
        mov->frag_index.item[index].headers_read = 1;
    }

    mov->found_mdat = 0;

    ret = mov_read_default(mov, s->pb, (MOVAtom){ AV_RL32("root"), INT64_MAX });
    if (ret < 0)
        return ret;
    if (avio_feof(s->pb))
        return AVERROR_EOF;
    av_log(s, AV_LOG_TRACE, "read fragments, offset 0x%"PRIx64"\n", avio_tell(s->pb));

    return 1;
}