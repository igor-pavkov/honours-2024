static MOVFragmentStreamInfo *get_frag_stream_info_from_pkt(MOVFragmentIndex *frag_index, AVPacket *pkt, int id)
{
    int current = frag_index->current;

    if (!frag_index->nb_items)
        return NULL;

    // Check frag_index->current is the right one for pkt. It can out of sync.
    if (current >= 0 && current < frag_index->nb_items) {
        if (frag_index->item[current].moof_offset < pkt->pos &&
            (current + 1 == frag_index->nb_items ||
             frag_index->item[current + 1].moof_offset > pkt->pos))
            return get_frag_stream_info(frag_index, current, id);
    }


    for (int i = 0; i < frag_index->nb_items; i++) {
        if (frag_index->item[i].moof_offset > pkt->pos)
            break;
        current = i;
    }
    frag_index->current = current;
    return get_frag_stream_info(frag_index, current, id);
}