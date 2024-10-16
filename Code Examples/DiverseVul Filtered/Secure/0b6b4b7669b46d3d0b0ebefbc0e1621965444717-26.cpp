static void sdap_initgr_rfc2307_process(struct tevent_req *subreq)
{
    struct tevent_req *req;
    struct sdap_initgr_rfc2307_state *state;
    struct sysdb_attrs **ldap_groups;
    char **sysdb_grouplist = NULL;
    size_t count;
    int ret;
    int i;

    req = tevent_req_callback_data(subreq, struct tevent_req);
    state = tevent_req_data(req, struct sdap_initgr_rfc2307_state);

    ret = sdap_get_generic_recv(subreq, state, &count, &ldap_groups);
    talloc_zfree(subreq);
    if (ret) {
        tevent_req_error(req, ret);
        return;
    }

    /* Add this batch of groups to the list */
    if (count > 0) {
        state->ldap_groups =
                talloc_realloc(state,
                               state->ldap_groups,
                               struct sysdb_attrs *,
                               state->ldap_groups_count + count + 1);
        if (!state->ldap_groups) {
            tevent_req_error(req, ENOMEM);
            return;
        }

        /* Copy the new groups into the list.
         */
        for (i = 0; i < count; i++) {
            state->ldap_groups[state->ldap_groups_count + i] =
                talloc_steal(state->ldap_groups, ldap_groups[i]);
        }

        state->ldap_groups_count += count;

        state->ldap_groups[state->ldap_groups_count] = NULL;
    }

    state->base_iter++;

    /* Check for additional search bases, and iterate
     * through again.
     */
    if (state->search_bases[state->base_iter] != NULL) {
        ret = sdap_initgr_rfc2307_next_base(req);
        if (ret != EOK) {
            tevent_req_error(req, ret);
        }
        return;
    }

    /* Search for all groups for which this user is a member */
    ret = get_sysdb_grouplist(state, state->sysdb, state->domain,
                              state->name, &sysdb_grouplist);
    if (ret != EOK) {
        tevent_req_error(req, ret);
        return;
    }

    /* There are no nested groups here so we can just update the
     * memberships */
    ret = sdap_initgr_common_store(state->sysdb,
                                   state->domain,
                                   state->opts,
                                   state->name,
                                   SYSDB_MEMBER_USER,
                                   sysdb_grouplist,
                                   state->ldap_groups,
                                   state->ldap_groups_count);
    if (ret != EOK) {
        tevent_req_error(req, ret);
        return;
    }

    tevent_req_done(req);
}