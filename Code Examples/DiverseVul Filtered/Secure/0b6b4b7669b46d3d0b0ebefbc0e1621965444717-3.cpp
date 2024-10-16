static errno_t get_sysdb_grouplist_ex(TALLOC_CTX *mem_ctx,
                                      struct sysdb_ctx *sysdb,
                                      struct sss_domain_info *domain,
                                      const char *name,
                                      char ***grouplist,
                                      bool get_dn)
{
    errno_t ret;
    const char *attrs[2];
    struct ldb_message *msg;
    TALLOC_CTX *tmp_ctx;
    struct ldb_message_element *groups;
    char **sysdb_grouplist = NULL;
    unsigned int i;

    attrs[0] = SYSDB_MEMBEROF;
    attrs[1] = NULL;

    tmp_ctx = talloc_new(NULL);
    if (!tmp_ctx) return ENOMEM;

    ret = sysdb_search_user_by_name(tmp_ctx, sysdb, domain, name,
                                    attrs, &msg);
    if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE,
              "Error searching user [%s] by name: [%s]\n",
               name, strerror(ret));
        goto done;
    }

    groups = ldb_msg_find_element(msg, SYSDB_MEMBEROF);
    if (!groups || groups->num_values == 0) {
        /* No groups for this user in sysdb currently */
        sysdb_grouplist = NULL;
    } else {
        sysdb_grouplist = talloc_array(tmp_ctx, char *, groups->num_values+1);
        if (!sysdb_grouplist) {
            ret = ENOMEM;
            goto done;
        }

        if (get_dn) {
            /* Get distinguish name */
            for (i=0; i < groups->num_values; i++) {
                sysdb_grouplist[i] = talloc_strdup(sysdb_grouplist,
                                       (const char *)groups->values[i].data);
                if (sysdb_grouplist[i] == NULL) {
                    ret = ENOMEM;
                    goto done;
                }
            }
        } else {
            /* Get a list of the groups by groupname only */
            for (i=0; i < groups->num_values; i++) {
                ret = sysdb_group_dn_name(sysdb,
                                          sysdb_grouplist,
                                          (const char *)groups->values[i].data,
                                          &sysdb_grouplist[i]);
                if (ret != EOK) {
                    DEBUG(SSSDBG_MINOR_FAILURE,
                          "Could not determine group name from [%s]: [%s]\n",
                           (const char *)groups->values[i].data, strerror(ret));
                    goto done;
                }
            }
        }

        sysdb_grouplist[groups->num_values] = NULL;
    }

    *grouplist = talloc_steal(mem_ctx, sysdb_grouplist);

done:
    talloc_free(tmp_ctx);
    return ret;
}