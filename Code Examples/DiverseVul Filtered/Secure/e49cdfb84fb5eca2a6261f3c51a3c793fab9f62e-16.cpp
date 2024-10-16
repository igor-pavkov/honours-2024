void ok_inflater_set_input(ok_inflater *inflater, const uint8_t *buffer, size_t buffer_length) {
    if (inflater) {
        if (inflater->input == inflater->input_end) {
            inflater->input = buffer;
            inflater->input_end = inflater->input + buffer_length;
        } else {
            ok_inflater_error(inflater, "ok_inflater_set_input was called with unread input data.");
        }
    }
}