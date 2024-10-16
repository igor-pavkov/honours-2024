static MOVFragmentStreamInfo * get_frag_stream_info(
    MOVFragmentIndex *frag_index,
    int index,
    int id)
{
    int i;
    MOVFragmentIndexItem * item;

    if (index < 0 || index >= frag_index->nb_items)
        return NULL;
    item = &frag_index->item[index];
    for (i = 0; i < item->nb_stream_info; i++)
        if (item->stream_info[i].id == id)
            return &item->stream_info[i];

    // This shouldn't happen
    return NULL;
}