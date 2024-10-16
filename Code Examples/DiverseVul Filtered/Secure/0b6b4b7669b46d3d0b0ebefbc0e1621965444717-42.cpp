errno_t save_rfc2307bis_user_memberships(
        struct sdap_initgr_rfc2307bis_state *state)
{
    errno_t ret, tret;
    char **ldap_grouplist;
    char **sysdb_parent_name_list;
    char **add_groups;
    char **del_groups;
    bool in_transaction = false;
    size_t c;
    char *tmp_str;

    TALLOC_CTX *tmp_ctx = talloc_new(NULL);
    if(!tmp_ctx) {
        return ENOMEM;
    }

    DEBUG(SSSDBG_TRACE_LIBS, "Save parent groups to sysdb\n");
    ret = sysdb_transaction_start(state->sysdb);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Failed to start transaction\n");
        goto error;
    }
    in_transaction = true;

    ret = sysdb_get_direct_parents(tmp_ctx, state->sysdb, state->dom,
                                   SYSDB_MEMBER_USER,
                                   state->name, &sysdb_parent_name_list);
    if (ret) {
        DEBUG(SSSDBG_CRIT_FAILURE,
              "Could not get direct sysdb parents for %s: %d [%s]\n",
                   state->name, ret, strerror(ret));
        goto error;
    }

    if (state->num_direct_parents == 0) {
        ldap_grouplist = NULL;
    }
    else {
        ret = sysdb_attrs_primary_name_list(
                state->sysdb, tmp_ctx,
                state->direct_groups, state->num_direct_parents,
                state->opts->group_map[SDAP_AT_GROUP_NAME].name,
                &ldap_grouplist);
        if (ret != EOK) {
            goto error;
        }

        if (IS_SUBDOMAIN(state->dom)) {
            for (c = 0; ldap_grouplist[c] != NULL; c++) {
                tmp_str = sss_tc_fqname(ldap_grouplist, state->dom->names,
                                        state->dom, ldap_grouplist[c]);
                if (tmp_str == NULL) {
                    DEBUG(SSSDBG_OP_FAILURE, "sss_tc_fqname failed.\n");
                    ret = ENOMEM;
                    goto error;
                }
                talloc_free(ldap_grouplist[c]);
                ldap_grouplist[c] = tmp_str;
            }
        }
    }

    /* Find the differences between the sysdb and ldap lists
     * Groups in ldap only must be added to the sysdb;
     * groups in the sysdb only must be removed.
     */
    ret = diff_string_lists(tmp_ctx,
                            ldap_grouplist, sysdb_parent_name_list,
                            &add_groups, &del_groups, NULL);
    if (ret != EOK) {
        goto error;
    }

    DEBUG(SSSDBG_TRACE_INTERNAL, "Updating memberships for %s\n", state->name);
    ret = sysdb_update_members(state->sysdb, state->dom,
                               state->name, SYSDB_MEMBER_USER,
                               (const char *const *)add_groups,
                               (const char *const *)del_groups);
    if (ret != EOK) {
        goto error;
    }

    ret = sysdb_transaction_commit(state->sysdb);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Failed to commit transaction\n");
        goto error;
    }
    in_transaction = false;

    talloc_free(tmp_ctx);
    return EOK;

error:
    if (in_transaction) {
        tret = sysdb_transaction_cancel(state->sysdb);
        if (tret != EOK) {
            DEBUG(SSSDBG_CRIT_FAILURE, "Failed to cancel transaction\n");
        }
    }
    talloc_free(tmp_ctx);
    return ret;
}