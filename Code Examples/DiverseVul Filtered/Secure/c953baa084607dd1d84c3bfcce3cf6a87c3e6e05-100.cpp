static void fix_frag_index_entries(MOVFragmentIndex *frag_index, int index,
                                   int id, int entries)
{
    int i;
    MOVFragmentStreamInfo * frag_stream_info;

    if (index < 0)
        return;
    for (i = index; i < frag_index->nb_items; i++) {
        frag_stream_info = get_frag_stream_info(frag_index, i, id);
        if (frag_stream_info && frag_stream_info->index_entry >= 0)
            frag_stream_info->index_entry += entries;
    }
}