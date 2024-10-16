static inline void get_lowest_part_y(H264Context *h, int refs[2][48], int n,
                                     int height, int y_offset, int list0,
                                     int list1, int *nrefs)
{
    int my;

    y_offset += 16 * (h->mb_y >> MB_FIELD(h));

    if (list0) {
        int ref_n    = h->ref_cache[0][scan8[n]];
        Picture *ref = &h->ref_list[0][ref_n];

        // Error resilience puts the current picture in the ref list.
        // Don't try to wait on these as it will cause a deadlock.
        // Fields can wait on each other, though.
        if (ref->tf.progress->data != h->cur_pic.tf.progress->data ||
            (ref->reference & 3) != h->picture_structure) {
            my = get_lowest_part_list_y(h, ref, n, height, y_offset, 0);
            if (refs[0][ref_n] < 0)
                nrefs[0] += 1;
            refs[0][ref_n] = FFMAX(refs[0][ref_n], my);
        }
    }

    if (list1) {
        int ref_n    = h->ref_cache[1][scan8[n]];
        Picture *ref = &h->ref_list[1][ref_n];

        if (ref->tf.progress->data != h->cur_pic.tf.progress->data ||
            (ref->reference & 3) != h->picture_structure) {
            my = get_lowest_part_list_y(h, ref, n, height, y_offset, 1);
            if (refs[1][ref_n] < 0)
                nrefs[1] += 1;
            refs[1][ref_n] = FFMAX(refs[1][ref_n], my);
        }
    }
}