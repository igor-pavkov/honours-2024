static int update_frag_index(MOVContext *c, int64_t offset)
{
    int index, i;
    MOVFragmentIndexItem * item;
    MOVFragmentStreamInfo * frag_stream_info;

    // If moof_offset already exists in frag_index, return index to it
    index = search_frag_moof_offset(&c->frag_index, offset);
    if (index < c->frag_index.nb_items &&
        c->frag_index.item[index].moof_offset == offset)
        return index;

    // offset is not yet in frag index.
    // Insert new item at index (sorted by moof offset)
    item = av_fast_realloc(c->frag_index.item,
                           &c->frag_index.allocated_size,
                           (c->frag_index.nb_items + 1) *
                           sizeof(*c->frag_index.item));
    if (!item)
        return -1;
    c->frag_index.item = item;

    frag_stream_info = av_realloc_array(NULL, c->fc->nb_streams,
                                        sizeof(*item->stream_info));
    if (!frag_stream_info)
        return -1;

    for (i = 0; i < c->fc->nb_streams; i++) {
        // Avoid building frag index if streams lack track id.
        if (c->fc->streams[i]->id < 0) {
            av_free(frag_stream_info);
            return AVERROR_INVALIDDATA;
        }

        frag_stream_info[i].id = c->fc->streams[i]->id;
        frag_stream_info[i].sidx_pts = AV_NOPTS_VALUE;
        frag_stream_info[i].tfdt_dts = AV_NOPTS_VALUE;
        frag_stream_info[i].next_trun_dts = AV_NOPTS_VALUE;
        frag_stream_info[i].first_tfra_pts = AV_NOPTS_VALUE;
        frag_stream_info[i].index_base = -1;
        frag_stream_info[i].index_entry = -1;
        frag_stream_info[i].encryption_index = NULL;
    }

    if (index < c->frag_index.nb_items)
        memmove(c->frag_index.item + index + 1, c->frag_index.item + index,
                (c->frag_index.nb_items - index) * sizeof(*c->frag_index.item));

    item = &c->frag_index.item[index];
    item->headers_read = 0;
    item->current = 0;
    item->nb_stream_info = c->fc->nb_streams;
    item->moof_offset = offset;
    item->stream_info = frag_stream_info;
    c->frag_index.nb_items++;

    return index;
}