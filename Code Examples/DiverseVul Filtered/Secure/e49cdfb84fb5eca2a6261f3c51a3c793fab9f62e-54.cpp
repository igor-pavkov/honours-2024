bool ok_inflater_needs_input(const ok_inflater *inflater) {
    return inflater &&
        inflater->state != OK_INFLATER_STATE_ERROR &&
        ok_inflater_can_flush_total(inflater) == 0 &&
        inflater->input == inflater->input_end;
}