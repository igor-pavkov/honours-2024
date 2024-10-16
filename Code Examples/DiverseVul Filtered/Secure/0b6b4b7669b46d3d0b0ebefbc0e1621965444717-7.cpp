static errno_t sdap_initgr_nested_deref_search(struct tevent_req *req)
{
    struct tevent_req *subreq;
    struct sdap_attr_map_info *maps;
    const int num_maps = 1;
    const char **sdap_attrs;
    errno_t ret;
    int timeout;
    struct sdap_initgr_nested_state *state;

    state = tevent_req_data(req, struct sdap_initgr_nested_state);

    maps = talloc_array(state, struct sdap_attr_map_info, num_maps+1);
    if (!maps) return ENOMEM;

    maps[0].map = state->opts->group_map;
    maps[0].num_attrs = SDAP_OPTS_GROUP;
    maps[1].map = NULL;

    ret = build_attrs_from_map(state, state->opts->group_map, SDAP_OPTS_GROUP,
                               NULL, &sdap_attrs, NULL);
    if (ret != EOK) goto fail;

    timeout = dp_opt_get_int(state->opts->basic, SDAP_SEARCH_TIMEOUT);

    subreq = sdap_deref_search_send(state, state->ev, state->opts,
                    state->sh, state->orig_dn,
                    state->opts->user_map[SDAP_AT_USER_MEMBEROF].name,
                    sdap_attrs, num_maps, maps, timeout);
    if (!subreq) {
        ret = EIO;
        goto fail;
    }
    talloc_steal(subreq, sdap_attrs);
    talloc_steal(subreq, maps);

    tevent_req_set_callback(subreq, sdap_initgr_nested_deref_done, req);
    return EAGAIN;

fail:
    talloc_free(sdap_attrs);
    talloc_free(maps);
    return ret;
}