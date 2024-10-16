static errno_t sdap_add_incomplete_groups(struct sysdb_ctx *sysdb,
                                          struct sss_domain_info *domain,
                                          struct sdap_options *opts,
                                          char **groupnames,
                                          struct sysdb_attrs **ldap_groups,
                                          int ldap_groups_count)
{
    TALLOC_CTX *tmp_ctx;
    struct ldb_message *msg;
    int i, mi, ai;
    const char *groupname;
    const char *original_dn;
    char **missing;
    gid_t gid;
    int ret;
    errno_t sret;
    bool in_transaction = false;
    bool posix;
    time_t now;
    char *sid_str = NULL;
    bool use_id_mapping;
    char *tmp_name;

    /* There are no groups in LDAP but we should add user to groups ?? */
    if (ldap_groups_count == 0) return EOK;

    tmp_ctx = talloc_new(NULL);
    if (!tmp_ctx) return ENOMEM;

    missing = talloc_array(tmp_ctx, char *, ldap_groups_count+1);
    if (!missing) {
        ret = ENOMEM;
        goto done;
    }
    mi = 0;

    for (i=0; groupnames[i]; i++) {
        tmp_name = sss_get_domain_name(tmp_ctx, groupnames[i], domain);
        if (tmp_name == NULL) {
            DEBUG(SSSDBG_OP_FAILURE,
                  "Failed to format original name [%s]\n", groupnames[i]);
            ret = ENOMEM;
            goto done;
        }

        ret = sysdb_search_group_by_name(tmp_ctx, sysdb, domain,
                                         tmp_name, NULL, &msg);
        if (ret == EOK) {
            continue;
        } else if (ret == ENOENT) {
            missing[mi] = talloc_steal(missing, tmp_name);
            DEBUG(SSSDBG_TRACE_LIBS, "Group #%d [%s][%s] is not cached, " \
                      "need to add a fake entry\n",
                      i, groupnames[i], missing[mi]);
            mi++;
            continue;
        } else if (ret != ENOENT) {
            DEBUG(SSSDBG_CRIT_FAILURE, "search for group failed [%d]: %s\n",
                      ret, strerror(ret));
            goto done;
        }
    }
    missing[mi] = NULL;

    /* All groups are cached, nothing to do */
    if (mi == 0) {
        ret = EOK;
        goto done;
    }

    use_id_mapping = sdap_idmap_domain_has_algorithmic_mapping(opts->idmap_ctx,
                                                             domain->name,
                                                             domain->domain_id);

    ret = sysdb_transaction_start(sysdb);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE,
              "Cannot start sysdb transaction [%d]: %s\n",
               ret, strerror(ret));
        goto done;
    }
    in_transaction = true;


    now = time(NULL);
    for (i=0; missing[i]; i++) {
        /* The group is not in sysdb, need to add a fake entry */
        for (ai=0; ai < ldap_groups_count; ai++) {
            ret = sdap_get_group_primary_name(tmp_ctx, opts, ldap_groups[ai],
                                              domain, &groupname);
            if (ret != EOK) {
                DEBUG(SSSDBG_CRIT_FAILURE,
                      "The group has no name attribute\n");
                goto done;
            }

            if (strcmp(groupname, missing[i]) == 0) {
                posix = true;

                ret = sdap_attrs_get_sid_str(
                        tmp_ctx, opts->idmap_ctx, ldap_groups[ai],
                        opts->group_map[SDAP_AT_GROUP_OBJECTSID].sys_name,
                        &sid_str);
                if (ret != EOK && ret != ENOENT) goto done;

                if (use_id_mapping) {
                    if (sid_str == NULL) {
                        DEBUG(SSSDBG_MINOR_FAILURE, "No SID for group [%s] " \
                                                     "while id-mapping.\n",
                                                     groupname);
                        ret = EINVAL;
                        goto done;
                    }

                    DEBUG(SSSDBG_TRACE_LIBS,
                          "Mapping group [%s] objectSID to unix ID\n", groupname);

                    DEBUG(SSSDBG_TRACE_INTERNAL,
                          "Group [%s] has objectSID [%s]\n",
                           groupname, sid_str);

                    /* Convert the SID into a UNIX group ID */
                    ret = sdap_idmap_sid_to_unix(opts->idmap_ctx, sid_str,
                                                 &gid);
                    if (ret == EOK) {
                        DEBUG(SSSDBG_TRACE_INTERNAL,
                              "Group [%s] has mapped gid [%lu]\n",
                               groupname, (unsigned long)gid);
                    } else {
                        posix = false;
                        gid = 0;

                        DEBUG(SSSDBG_TRACE_INTERNAL,
                              "Group [%s] cannot be mapped. "
                               "Treating as a non-POSIX group\n",
                               groupname);
                    }

                } else {
                    ret = sysdb_attrs_get_uint32_t(ldap_groups[ai],
                                                   SYSDB_GIDNUM,
                                                   &gid);
                    if (ret == ENOENT || (ret == EOK && gid == 0)) {
                        DEBUG(SSSDBG_TRACE_LIBS, "The group %s gid was %s\n",
                              groupname, ret == ENOENT ? "missing" : "zero");
                        DEBUG(SSSDBG_TRACE_FUNC,
                              "Marking group %s as non-posix and setting GID=0!\n",
                              groupname);
                        gid = 0;
                        posix = false;
                    } else if (ret) {
                        DEBUG(SSSDBG_CRIT_FAILURE,
                              "The GID attribute is malformed\n");
                        goto done;
                    }
                }

                ret = sysdb_attrs_get_string(ldap_groups[ai],
                                             SYSDB_ORIG_DN,
                                             &original_dn);
                if (ret) {
                    DEBUG(SSSDBG_FUNC_DATA,
                          "The group has no name original DN\n");
                    original_dn = NULL;
                }

                DEBUG(SSSDBG_TRACE_INTERNAL,
                      "Adding fake group %s to sysdb\n", groupname);
                ret = sysdb_add_incomplete_group(sysdb, domain, groupname, gid,
                                                 original_dn, sid_str, posix,
                                                 now);
                if (ret != EOK) {
                    goto done;
                }
                break;
            }
        }

        if (ai == ldap_groups_count) {
            DEBUG(SSSDBG_OP_FAILURE,
                  "Group %s not present in LDAP\n", missing[i]);
            ret = EINVAL;
            goto done;
        }
    }

    ret = sysdb_transaction_commit(sysdb);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "sysdb_transaction_commit failed.\n");
        goto done;
    }
    in_transaction = false;
    ret = EOK;

done:
    if (in_transaction) {
        sret = sysdb_transaction_cancel(sysdb);
        if (sret != EOK) {
            DEBUG(SSSDBG_CRIT_FAILURE, "Failed to cancel transaction\n");
        }
    }
    talloc_free(tmp_ctx);
    return ret;
}