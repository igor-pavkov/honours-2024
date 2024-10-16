static int mov_seek_fragment(AVFormatContext *s, AVStream *st, int64_t timestamp)
{
    MOVContext *mov = s->priv_data;
    int index;

    if (!mov->frag_index.complete)
        return 0;

    index = search_frag_timestamp(s, &mov->frag_index, st, timestamp);
    if (index < 0)
        index = 0;
    if (!mov->frag_index.item[index].headers_read)
        return mov_switch_root(s, -1, index);
    if (index + 1 < mov->frag_index.nb_items)
        mov->next_root_atom = mov->frag_index.item[index + 1].moof_offset;

    return 0;
}