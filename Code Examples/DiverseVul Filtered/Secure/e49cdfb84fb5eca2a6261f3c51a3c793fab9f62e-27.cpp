void ok_inflater_reset(ok_inflater *inflater) {
    if (inflater) {
        inflater->input = NULL;
        inflater->input_end = NULL;
        inflater->input_buffer = 0;
        inflater->input_buffer_bits = 0;

        inflater->buffer_start_pos = 0;
        inflater->buffer_end_pos = 0;
        inflater->final_block = false;
        inflater->state = (inflater->nowrap ? OK_INFLATER_STATE_READY_FOR_NEXT_BLOCK :
                           OK_INFLATER_STATE_READY_FOR_HEAD);
    }
}