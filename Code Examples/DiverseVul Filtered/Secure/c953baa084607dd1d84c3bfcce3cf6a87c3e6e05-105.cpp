static int64_t add_ctts_entry(MOVCtts** ctts_data, unsigned int* ctts_count, unsigned int* allocated_size,
                              int count, int duration)
{
    MOVCtts *ctts_buf_new;
    const size_t min_size_needed = (*ctts_count + 1) * sizeof(MOVCtts);
    const size_t requested_size =
        min_size_needed > *allocated_size ?
        FFMAX(min_size_needed, 2 * (*allocated_size)) :
        min_size_needed;

    if ((unsigned)(*ctts_count) >= UINT_MAX / sizeof(MOVCtts) - 1)
        return -1;

    ctts_buf_new = av_fast_realloc(*ctts_data, allocated_size, requested_size);

    if (!ctts_buf_new)
        return -1;

    *ctts_data = ctts_buf_new;

    ctts_buf_new[*ctts_count].count = count;
    ctts_buf_new[*ctts_count].duration = duration;

    *ctts_count = (*ctts_count) + 1;
    return *ctts_count;
}