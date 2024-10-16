static void sdap_initgr_rfc2307bis_process(struct tevent_req *subreq)
{
    struct tevent_req *req;
    struct sdap_initgr_rfc2307bis_state *state;
    struct sysdb_attrs **ldap_groups;
    size_t count;
    size_t i;
    int ret;

    req = tevent_req_callback_data(subreq, struct tevent_req);
    state = tevent_req_data(req, struct sdap_initgr_rfc2307bis_state);

    ret = sdap_get_generic_recv(subreq, state,
                                &count,
                                &ldap_groups);
    talloc_zfree(subreq);
    if (ret) {
        tevent_req_error(req, ret);
        return;
    }
    DEBUG(SSSDBG_TRACE_LIBS,
          "Found %zu parent groups for user [%s]\n", count, state->name);

    /* Add this batch of groups to the list */
    if (count > 0) {
        state->direct_groups =
                talloc_realloc(state,
                               state->direct_groups,
                               struct sysdb_attrs *,
                               state->num_direct_parents + count + 1);
        if (!state->direct_groups) {
            tevent_req_error(req, ENOMEM);
            return;
        }

        /* Copy the new groups into the list.
         */
        for (i = 0; i < count; i++) {
            state->direct_groups[state->num_direct_parents + i] =
                    talloc_steal(state->direct_groups, ldap_groups[i]);
        }

        state->num_direct_parents += count;

        state->direct_groups[state->num_direct_parents] = NULL;
    }

    state->base_iter++;

    /* Check for additional search bases, and iterate
     * through again.
     */
    if (state->search_bases[state->base_iter] != NULL) {
        ret = sdap_initgr_rfc2307bis_next_base(req);
        if (ret != EOK) {
            tevent_req_error(req, ret);
        }
        return;
    }

    if (state->num_direct_parents == 0) {
        /* Start a transaction to look up the groups in the sysdb
         * and update them with LDAP data
         */
        ret = save_rfc2307bis_user_memberships(state);
        if (ret != EOK) {
            tevent_req_error(req, ret);
        } else {
            tevent_req_done(req);
        }
        return;
    }

    subreq = rfc2307bis_nested_groups_send(state, state->ev, state->opts,
                                           state->sysdb, state->dom,
                                           state->sh, state->direct_groups,
                                           state->num_direct_parents,
                                           state->group_hash, 0);
    if (!subreq) {
        tevent_req_error(req, EIO);
        return;
    }
    tevent_req_set_callback(subreq, sdap_initgr_rfc2307bis_done, req);
}