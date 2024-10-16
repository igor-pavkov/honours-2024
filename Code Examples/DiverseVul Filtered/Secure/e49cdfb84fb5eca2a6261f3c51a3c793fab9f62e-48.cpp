static bool ok_inflater_stored_block(ok_inflater *inflater) {
    const intptr_t can_read = inflater->input_end - inflater->input;
    if (can_read == 0) {
        return false;
    } else {
        int len = ok_inflater_write_bytes(inflater, inflater->input,
                                          min((int)can_read, inflater->state_count));
        if (len == 0) {
            // Buffer full
            return false;
        }
        inflater->input += len;
        inflater->state_count -= len;
        if (inflater->state_count == 0) {
            inflater->state = OK_INFLATER_STATE_READY_FOR_NEXT_BLOCK;
        }
        return true;
    }
}