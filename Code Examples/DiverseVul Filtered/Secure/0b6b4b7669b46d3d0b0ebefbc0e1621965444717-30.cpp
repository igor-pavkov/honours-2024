static int sdap_initgr_nested_get_direct_parents(TALLOC_CTX *mem_ctx,
                                                 struct sysdb_attrs *attrs,
                                                 struct sysdb_attrs **groups,
                                                 int ngroups,
                                                 struct sysdb_attrs ***_direct_parents,
                                                 int *_ndirect)
{
    TALLOC_CTX *tmp_ctx;
    struct ldb_message_element *member;
    int i, mi;
    int ret;
    const char *orig_dn;

    int ndirect;
    struct sysdb_attrs **direct_groups;

    tmp_ctx = talloc_new(NULL);
    if (!tmp_ctx) return ENOMEM;

    direct_groups = talloc_zero_array(tmp_ctx, struct sysdb_attrs *,
                                      ngroups + 1);
    if (!direct_groups) {
        ret = ENOMEM;
        goto done;
    }
    ndirect = 0;

    ret = sysdb_attrs_get_string(attrs, SYSDB_ORIG_DN, &orig_dn);
    if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE, "Missing originalDN\n");
        goto done;
    }
    DEBUG(SSSDBG_TRACE_ALL,
          "Looking up direct parents for group [%s]\n", orig_dn);

    /* FIXME - Filter only parents from full set to avoid searching
     * through all members of huge groups. That requires asking for memberOf
     * with the group LDAP search
     */

    /* Filter only direct parents from the list of all groups */
    for (i=0; i < ngroups; i++) {
        ret = sysdb_attrs_get_el(groups[i], SYSDB_MEMBER, &member);
        if (ret) {
            DEBUG(SSSDBG_TRACE_LIBS,
                  "A group with no members during initgroups?\n");
            continue;
        }

        for (mi = 0; mi < member->num_values; mi++) {
            if (strcasecmp((const char *) member->values[mi].data, orig_dn) != 0) {
                continue;
            }

            direct_groups[ndirect] = groups[i];
            ndirect++;
        }
    }
    direct_groups[ndirect] = NULL;

    DEBUG(SSSDBG_TRACE_ALL,
          "The group [%s] has %d direct parents\n", orig_dn, ndirect);

    *_direct_parents = talloc_steal(mem_ctx, direct_groups);
    *_ndirect = ndirect;
    ret = EOK;
done:
    talloc_zfree(tmp_ctx);
    return ret;
}