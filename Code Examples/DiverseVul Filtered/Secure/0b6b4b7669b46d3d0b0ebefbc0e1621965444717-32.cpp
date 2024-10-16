int sdap_initgr_common_store(struct sysdb_ctx *sysdb,
                             struct sss_domain_info *domain,
                             struct sdap_options *opts,
                             const char *name,
                             enum sysdb_member_type type,
                             char **sysdb_grouplist,
                             struct sysdb_attrs **ldap_groups,
                             int ldap_groups_count)
{
    TALLOC_CTX *tmp_ctx;
    char **ldap_grouplist = NULL;
    char **add_groups;
    char **del_groups;
    int ret, tret;
    bool in_transaction = false;

    tmp_ctx = talloc_new(NULL);
    if (!tmp_ctx) return ENOMEM;

    if (ldap_groups_count == 0) {
        /* No groups for this user in LDAP.
         * We need to ensure that there are no groups
         * in the sysdb either.
         */
        ldap_grouplist = NULL;
    } else {
        ret = sysdb_attrs_primary_name_list(
                sysdb, tmp_ctx,
                ldap_groups, ldap_groups_count,
                opts->group_map[SDAP_AT_GROUP_NAME].name,
                &ldap_grouplist);
        if (ret != EOK) {
            DEBUG(SSSDBG_CRIT_FAILURE,
                  "sysdb_attrs_primary_name_list failed [%d]: %s\n",
                      ret, strerror(ret));
            goto done;
        }
    }

    /* Find the differences between the sysdb and LDAP lists
     * Groups in the sysdb only must be removed.
     */
    ret = diff_string_lists(tmp_ctx, ldap_grouplist, sysdb_grouplist,
                            &add_groups, &del_groups, NULL);
    if (ret != EOK) goto done;

    ret = sysdb_transaction_start(sysdb);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Failed to start transaction\n");
        goto done;
    }
    in_transaction = true;

    /* Add fake entries for any groups the user should be added as
     * member of but that are not cached in sysdb
     */
    if (add_groups && add_groups[0]) {
        ret = sdap_add_incomplete_groups(sysdb, domain, opts,
                                         add_groups, ldap_groups,
                                         ldap_groups_count);
        if (ret != EOK) {
            DEBUG(SSSDBG_CRIT_FAILURE, "Adding incomplete users failed\n");
            goto done;
        }
    }

    DEBUG(SSSDBG_TRACE_INTERNAL, "Updating memberships for %s\n", name);
    ret = sysdb_update_members(sysdb, domain, name, type,
                               (const char *const *) add_groups,
                               (const char *const *) del_groups);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Membership update failed [%d]: %s\n",
                  ret, strerror(ret));
        goto done;
    }

    ret = sysdb_transaction_commit(sysdb);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Failed to commit transaction\n");
        goto done;
    }
    in_transaction = false;

    ret = EOK;
done:
    if (in_transaction) {
        tret = sysdb_transaction_cancel(sysdb);
        if (tret != EOK) {
            DEBUG(SSSDBG_CRIT_FAILURE, "Failed to cancel transaction\n");
        }
    }
    talloc_zfree(tmp_ctx);
    return ret;
}