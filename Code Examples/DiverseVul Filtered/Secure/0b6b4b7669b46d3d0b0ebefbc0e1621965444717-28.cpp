static errno_t sdap_initgr_nested_noderef_search(struct tevent_req *req)
{
    int i;
    struct tevent_req *subreq;
    struct sdap_initgr_nested_state *state;

    state = tevent_req_data(req, struct sdap_initgr_nested_state);

    state->group_dns = talloc_array(state, char *,
                                    state->memberof->num_values + 1);
    if (!state->group_dns) {
        return ENOMEM;
    }
    for (i = 0; i < state->memberof->num_values; i++) {
        state->group_dns[i] = talloc_strdup(state->group_dns,
                                    (char *)state->memberof->values[i].data);
        if (!state->group_dns[i]) {
            return ENOMEM;
        }
    }
    state->group_dns[i] = NULL; /* terminate */
    state->cur = 0;

    state->filter = talloc_asprintf(state, "(&(objectclass=%s)(%s=*))",
                            state->opts->group_map[SDAP_OC_GROUP].name,
                            state->opts->group_map[SDAP_AT_GROUP_NAME].name);
    if (!state->filter) {
        return ENOMEM;
    }

    subreq = sdap_get_generic_send(state, state->ev, state->opts, state->sh,
                                   state->group_dns[state->cur],
                                   LDAP_SCOPE_BASE,
                                   state->filter, state->grp_attrs,
                                   state->opts->group_map, SDAP_OPTS_GROUP,
                                   dp_opt_get_int(state->opts->basic,
                                                  SDAP_SEARCH_TIMEOUT),
                                   false);
    if (!subreq) {
        return ENOMEM;
    }
    tevent_req_set_callback(subreq, sdap_initgr_nested_search, req);

    return EAGAIN;
}