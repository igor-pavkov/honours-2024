static int search_frag_timestamp(AVFormatContext *s, MOVFragmentIndex *frag_index,
                                 AVStream *st, int64_t timestamp)
{
    int a, b, m, m0;
    int64_t frag_time;

    a = -1;
    b = frag_index->nb_items;

    while (b - a > 1) {
        m0 = m = (a + b) >> 1;

        while (m < b &&
               (frag_time = get_frag_time(s, st, frag_index, m)) == AV_NOPTS_VALUE)
            m++;

        if (m < b && frag_time <= timestamp)
            a = m;
        else
            b = m0;
    }

    return a;
}