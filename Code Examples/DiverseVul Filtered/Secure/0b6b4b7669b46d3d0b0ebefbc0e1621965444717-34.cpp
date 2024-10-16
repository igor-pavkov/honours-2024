struct tevent_req *sdap_get_initgr_send(TALLOC_CTX *memctx,
                                        struct tevent_context *ev,
                                        struct sdap_domain *sdom,
                                        struct sdap_handle *sh,
                                        struct sdap_id_ctx *id_ctx,
                                        struct sdap_id_conn_ctx *conn,
                                        const char *name,
                                        const char **grp_attrs)
{
    struct tevent_req *req;
    struct sdap_get_initgr_state *state;
    int ret;
    char *clean_name;
    bool use_id_mapping;

    DEBUG(SSSDBG_TRACE_ALL, "Retrieving info for initgroups call\n");

    req = tevent_req_create(memctx, &state, struct sdap_get_initgr_state);
    if (!req) return NULL;

    state->ev = ev;
    state->opts = id_ctx->opts;
    state->dom = sdom->dom;
    state->sysdb = sdom->dom->sysdb;
    state->sdom = sdom;
    state->sh = sh;
    state->id_ctx = id_ctx;
    state->conn = conn;
    state->name = name;
    state->grp_attrs = grp_attrs;
    state->orig_user = NULL;
    state->timeout = dp_opt_get_int(state->opts->basic, SDAP_SEARCH_TIMEOUT);
    state->user_base_iter = 0;
    state->user_search_bases = sdom->user_search_bases;
    if (!state->user_search_bases) {
        DEBUG(SSSDBG_CRIT_FAILURE,
              "Initgroups lookup request without a user search base\n");
        ret = EINVAL;
        goto done;
    }

    use_id_mapping = sdap_idmap_domain_has_algorithmic_mapping(
                                                          id_ctx->opts->idmap_ctx,
                                                          sdom->dom->name,
                                                          sdom->dom->domain_id);

    ret = sss_filter_sanitize(state, name, &clean_name);
    if (ret != EOK) {
        talloc_zfree(req);
        return NULL;
    }

    state->user_base_filter =
            talloc_asprintf(state, "(&(%s=%s)(objectclass=%s)",
                            state->opts->user_map[SDAP_AT_USER_NAME].name,
                            clean_name,
                            state->opts->user_map[SDAP_OC_USER].name);
    if (!state->user_base_filter) {
        talloc_zfree(req);
        return NULL;
    }

    if (use_id_mapping) {
        /* When mapping IDs or looking for SIDs, we don't want to limit
         * ourselves to users with a UID value. But there must be a SID to map
         * from.
         */
        state->user_base_filter = talloc_asprintf_append(state->user_base_filter,
                                        "(%s=*))",
                                        id_ctx->opts->user_map[SDAP_AT_USER_OBJECTSID].name);
    } else {
        /* When not ID-mapping, make sure there is a non-NULL UID */
        state->user_base_filter = talloc_asprintf_append(state->user_base_filter,
                                        "(&(%s=*)(!(%s=0))))",
                                        id_ctx->opts->user_map[SDAP_AT_USER_UID].name,
                                        id_ctx->opts->user_map[SDAP_AT_USER_UID].name);
    }
    if (!state->user_base_filter) {
        talloc_zfree(req);
        return NULL;
    }

    ret = build_attrs_from_map(state,
                               state->opts->user_map,
                               state->opts->user_map_cnt,
                               NULL, &state->user_attrs, NULL);
    if (ret) {
        talloc_zfree(req);
        return NULL;
    }

    state->use_id_mapping = sdap_idmap_domain_has_algorithmic_mapping(
                                                         state->opts->idmap_ctx,
                                                         state->dom->name,
                                                         state->dom->domain_id);

    ret = sdap_get_initgr_next_base(req);

done:
    if (ret != EOK) {
        tevent_req_error(req, ret);
        tevent_req_post(req, ev);
    }

    return req;
}