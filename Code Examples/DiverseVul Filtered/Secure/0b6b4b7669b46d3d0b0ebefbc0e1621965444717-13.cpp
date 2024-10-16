struct tevent_req *rfc2307bis_nested_groups_send(
        TALLOC_CTX *mem_ctx, struct tevent_context *ev,
        struct sdap_options *opts, struct sysdb_ctx *sysdb,
        struct sss_domain_info *dom, struct sdap_handle *sh,
        struct sysdb_attrs **groups, size_t num_groups,
        hash_table_t *group_hash, size_t nesting)
{
    errno_t ret;
    struct tevent_req *req;
    struct sdap_rfc2307bis_nested_ctx *state;

    DEBUG(SSSDBG_TRACE_INTERNAL,
          "About to process %zu groups in nesting level %zu\n",
           num_groups, nesting);

    req = tevent_req_create(mem_ctx, &state,
                            struct sdap_rfc2307bis_nested_ctx);
    if (!req) return NULL;

    if ((num_groups == 0) ||
        (nesting > dp_opt_get_int(opts->basic, SDAP_NESTING_LEVEL))) {
        /* No parent groups to process or too deep*/
        ret = EOK;
        goto done;
    }

    state->ev = ev;
    state->opts = opts;
    state->sysdb = sysdb;
    state->dom = dom;
    state->sh = sh;
    state->groups = groups;
    state->num_groups = num_groups;
    state->group_iter = 0;
    state->nesting_level = nesting;
    state->group_hash = group_hash;
    state->filter = NULL;
    state->timeout = dp_opt_get_int(state->opts->basic,
                                    SDAP_SEARCH_TIMEOUT);
    state->base_iter = 0;
    state->search_bases = opts->sdom->group_search_bases;
    if (!state->search_bases) {
        DEBUG(SSSDBG_CRIT_FAILURE,
              "Initgroups nested lookup request "
               "without a group search base\n");
        ret = EINVAL;
        goto done;
    }

    state->processed_groups = talloc_array(state,
                                           struct sdap_nested_group *,
                                           state->num_groups);
    if (state->processed_groups == NULL) {
        ret = ENOMEM;
        goto done;
    }

    while (state->group_iter < state->num_groups) {
        ret = rfc2307bis_nested_groups_step(req);
        if (ret == EOK) {
            /* This group had already been looked up. Continue to
             * another group in the same level
             */
            state->group_iter++;
            continue;
        } else {
            goto done;
        }
    }

    ret = EOK;

done:
    if (ret == EOK) {
        /* All parent groups were already processed */
        tevent_req_done(req);
        tevent_req_post(req, ev);
    } else if (ret != EAGAIN) {
        tevent_req_error(req, ret);
        tevent_req_post(req, ev);
    }

    /* EAGAIN means a lookup is in progress */
    return req;
}