static void set_frag_stream(MOVFragmentIndex *frag_index, int id)
{
    int i;
    MOVFragmentIndexItem * item;

    if (frag_index->current < 0 ||
        frag_index->current >= frag_index->nb_items)
        return;

    item = &frag_index->item[frag_index->current];
    for (i = 0; i < item->nb_stream_info; i++)
        if (item->stream_info[i].id == id) {
            item->current = i;
            return;
        }

    // id not found.  This shouldn't happen.
    item->current = -1;
}