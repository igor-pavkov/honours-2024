static void connection_set_state_error(request_st * const r, const request_state_t state) {
    connection_set_state(r, state);
}