static void fix_index_entry_timestamps(AVStream* st, int end_index, int64_t end_ts,
                                       int64_t* frame_duration_buffer,
                                       int frame_duration_buffer_size) {
    FFStream *const sti = ffstream(st);
    int i = 0;
    av_assert0(end_index >= 0 && end_index <= sti->nb_index_entries);
    for (i = 0; i < frame_duration_buffer_size; i++) {
        end_ts -= frame_duration_buffer[frame_duration_buffer_size - 1 - i];
        sti->index_entries[end_index - 1 - i].timestamp = end_ts;
    }
}