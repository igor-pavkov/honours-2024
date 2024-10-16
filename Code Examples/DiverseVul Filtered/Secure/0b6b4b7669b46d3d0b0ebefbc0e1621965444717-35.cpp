struct tevent_req *sdap_initgr_rfc2307_send(TALLOC_CTX *memctx,
                                            struct tevent_context *ev,
                                            struct sdap_options *opts,
                                            struct sysdb_ctx *sysdb,
                                            struct sss_domain_info *domain,
                                            struct sdap_handle *sh,
                                            const char *name)
{
    struct tevent_req *req;
    struct sdap_initgr_rfc2307_state *state;
    const char **attr_filter;
    char *clean_name;
    errno_t ret;

    req = tevent_req_create(memctx, &state, struct sdap_initgr_rfc2307_state);
    if (!req) return NULL;

    state->ev = ev;
    state->opts = opts;
    state->sysdb = sysdb;
    state->domain = domain;
    state->sh = sh;
    state->op = NULL;
    state->timeout = dp_opt_get_int(state->opts->basic, SDAP_SEARCH_TIMEOUT);
    state->ldap_groups = NULL;
    state->ldap_groups_count = 0;
    state->base_iter = 0;
    state->search_bases = opts->sdom->group_search_bases;

    if (!state->search_bases) {
        DEBUG(SSSDBG_CRIT_FAILURE,
              "Initgroups lookup request without a group search base\n");
        ret = EINVAL;
        goto done;
    }

    state->name = talloc_strdup(state, name);
    if (!state->name) {
        talloc_zfree(req);
        return NULL;
    }

    attr_filter = talloc_array(state, const char *, 2);
    if (!attr_filter) {
        talloc_free(req);
        return NULL;
    }

    attr_filter[0] = opts->group_map[SDAP_AT_GROUP_MEMBER].name;
    attr_filter[1] = NULL;

    ret = build_attrs_from_map(state, opts->group_map, SDAP_OPTS_GROUP,
                               attr_filter, &state->attrs, NULL);
    if (ret != EOK) {
        talloc_free(req);
        return NULL;
    }

    ret = sss_filter_sanitize(state, name, &clean_name);
    if (ret != EOK) {
        talloc_free(req);
        return NULL;
    }

    state->base_filter = talloc_asprintf(state,
                             "(&(%s=%s)(objectclass=%s)(%s=*)(&(%s=*)(!(%s=0))))",
                             opts->group_map[SDAP_AT_GROUP_MEMBER].name,
                             clean_name,
                             opts->group_map[SDAP_OC_GROUP].name,
                             opts->group_map[SDAP_AT_GROUP_NAME].name,
                             opts->group_map[SDAP_AT_GROUP_GID].name,
                             opts->group_map[SDAP_AT_GROUP_GID].name);
    if (!state->base_filter) {
        talloc_zfree(req);
        return NULL;
    }
    talloc_zfree(clean_name);

    ret = sdap_initgr_rfc2307_next_base(req);

done:
    if (ret != EOK) {
        tevent_req_error(req, ret);
        tevent_req_post(req, ev);
    }

    return req;
}