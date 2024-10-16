static errno_t sdap_get_initgr_next_base(struct tevent_req *req)
{
    struct tevent_req *subreq;
    struct sdap_get_initgr_state *state;

    state = tevent_req_data(req, struct sdap_get_initgr_state);

    talloc_zfree(state->filter);
    state->filter = sdap_get_id_specific_filter(
            state,
            state->user_base_filter,
            state->user_search_bases[state->user_base_iter]->filter);
    if (!state->filter) {
        return ENOMEM;
    }

    DEBUG(SSSDBG_TRACE_FUNC,
          "Searching for users with base [%s]\n",
           state->user_search_bases[state->user_base_iter]->basedn);

    subreq = sdap_get_generic_send(
            state, state->ev, state->opts, state->sh,
            state->user_search_bases[state->user_base_iter]->basedn,
            state->user_search_bases[state->user_base_iter]->scope,
            state->filter, state->user_attrs,
            state->opts->user_map, state->opts->user_map_cnt,
            state->timeout,
            false);
    if (!subreq) {
        return ENOMEM;
    }
    tevent_req_set_callback(subreq, sdap_get_initgr_user, req);
    return EOK;
}