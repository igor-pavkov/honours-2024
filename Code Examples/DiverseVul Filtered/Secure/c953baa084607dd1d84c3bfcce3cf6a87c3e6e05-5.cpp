static int search_frag_moof_offset(MOVFragmentIndex *frag_index, int64_t offset)
{
    int a, b, m;
    int64_t moof_offset;

    // Optimize for appending new entries
    if (!frag_index->nb_items ||
        frag_index->item[frag_index->nb_items - 1].moof_offset < offset)
        return frag_index->nb_items;

    a = -1;
    b = frag_index->nb_items;

    while (b - a > 1) {
        m = (a + b) >> 1;
        moof_offset = frag_index->item[m].moof_offset;
        if (moof_offset >= offset)
            b = m;
        if (moof_offset <= offset)
            a = m;
    }
    return b;
}