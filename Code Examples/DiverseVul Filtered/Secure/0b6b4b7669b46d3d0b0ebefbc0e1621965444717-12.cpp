static errno_t sdap_initgr_rfc2307bis_next_base(struct tevent_req *req)
{
    struct tevent_req *subreq;
    struct sdap_initgr_rfc2307bis_state *state;

    state = tevent_req_data(req, struct sdap_initgr_rfc2307bis_state);

    talloc_zfree(state->filter);
    state->filter = sdap_get_id_specific_filter(
            state,
            state->base_filter,
            state->search_bases[state->base_iter]->filter);
    if (!state->filter) {
        return ENOMEM;
    }

    DEBUG(SSSDBG_TRACE_FUNC,
          "Searching for parent groups for user [%s] with base [%s]\n",
           state->orig_dn, state->search_bases[state->base_iter]->basedn);

    subreq = sdap_get_generic_send(
            state, state->ev, state->opts, state->sh,
            state->search_bases[state->base_iter]->basedn,
            state->search_bases[state->base_iter]->scope,
            state->filter, state->attrs,
            state->opts->group_map, SDAP_OPTS_GROUP,
            state->timeout,
            true);
    if (!subreq) {
        talloc_zfree(req);
        return ENOMEM;
    }
    tevent_req_set_callback(subreq, sdap_initgr_rfc2307bis_process, req);

    return EOK;
}