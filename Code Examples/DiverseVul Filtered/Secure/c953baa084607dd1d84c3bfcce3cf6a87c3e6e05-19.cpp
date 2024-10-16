static int64_t add_index_entry(AVStream *st, int64_t pos, int64_t timestamp,
                               int size, int distance, int flags)
{
    FFStream *const sti = ffstream(st);
    AVIndexEntry *entries, *ie;
    int64_t index = -1;
    const size_t min_size_needed = (sti->nb_index_entries + 1) * sizeof(AVIndexEntry);

    // Double the allocation each time, to lower memory fragmentation.
    // Another difference from ff_add_index_entry function.
    const size_t requested_size =
        min_size_needed > sti->index_entries_allocated_size ?
        FFMAX(min_size_needed, 2 * sti->index_entries_allocated_size) :
        min_size_needed;

    if (sti->nb_index_entries + 1U >= UINT_MAX / sizeof(AVIndexEntry))
        return -1;

    entries = av_fast_realloc(sti->index_entries,
                              &sti->index_entries_allocated_size,
                              requested_size);
    if (!entries)
        return -1;

    sti->index_entries = entries;

    index = sti->nb_index_entries++;
    ie= &entries[index];

    ie->pos = pos;
    ie->timestamp = timestamp;
    ie->min_distance= distance;
    ie->size= size;
    ie->flags = flags;
    return index;
}