static errno_t rfc2307bis_nested_groups_step(struct tevent_req *req)
{
    errno_t ret;
    TALLOC_CTX *tmp_ctx = NULL;
    const char **attr_filter;
    char *clean_orig_dn;
    hash_key_t key;
    hash_value_t value;
    struct sdap_rfc2307bis_nested_ctx *state =
            tevent_req_data(req, struct sdap_rfc2307bis_nested_ctx);

    tmp_ctx = talloc_new(state);
    if (!tmp_ctx) {
        ret = ENOMEM;
        goto done;
    }

    ret = sdap_get_group_primary_name(state, state->opts,
                                      state->groups[state->group_iter],
                                      state->dom, &state->primary_name);
    if (ret != EOK) {
        goto done;
    }

    key.type = HASH_KEY_STRING;
    key.str = talloc_strdup(state, state->primary_name);
    if (!key.str) {
        ret = ENOMEM;
        goto done;
    }

    DEBUG(SSSDBG_TRACE_LIBS, "Processing group [%s]\n", state->primary_name);

    ret = hash_lookup(state->group_hash, &key, &value);
    if (ret == HASH_SUCCESS) {
        DEBUG(SSSDBG_TRACE_INTERNAL, "Group [%s] was already processed, "
              "taking a shortcut\n", state->primary_name);
        state->processed_groups[state->group_iter] =
            talloc_get_type(value.ptr, struct sdap_nested_group);
        talloc_free(key.str);
        ret = EOK;
        goto done;
    }

    /* Need to try to find parent groups for this group. */
    state->processed_groups[state->group_iter] =
            talloc_zero(state->processed_groups, struct sdap_nested_group);
    if (!state->processed_groups[state->group_iter]) {
        ret = ENOMEM;
        goto done;
    }

    /* this steal doesn't change much now, but will be helpful later on
     * if we steal the whole processed_group on the hash table */
    state->processed_groups[state->group_iter]->group =
        talloc_steal(state->processed_groups[state->group_iter],
                     state->groups[state->group_iter]);

    /* Get any parent groups for this group */
    ret = sysdb_attrs_get_string(state->groups[state->group_iter],
                                 SYSDB_ORIG_DN,
                                 &state->orig_dn);
    if (ret != EOK) {
        goto done;
    }

    attr_filter = talloc_array(state, const char *, 2);
    if (!attr_filter) {
        ret = ENOMEM;
        goto done;
    }

    attr_filter[0] = state->opts->group_map[SDAP_AT_GROUP_MEMBER].name;
    attr_filter[1] = NULL;

    ret = build_attrs_from_map(state, state->opts->group_map, SDAP_OPTS_GROUP,
                               attr_filter, &state->attrs, NULL);
    if (ret != EOK) {
        goto done;
    }

    ret = sss_filter_sanitize(tmp_ctx, state->orig_dn, &clean_orig_dn);
    if (ret != EOK) {
        goto done;
    }

    state->base_filter = talloc_asprintf(
            state, "(&(%s=%s)(objectclass=%s)(%s=*))",
            state->opts->group_map[SDAP_AT_GROUP_MEMBER].name,
            clean_orig_dn,
            state->opts->group_map[SDAP_OC_GROUP].name,
            state->opts->group_map[SDAP_AT_GROUP_NAME].name);
    if (!state->base_filter) {
        ret = ENOMEM;
        goto done;
    }

    ret = rfc2307bis_nested_groups_next_base(req);
    if (ret != EOK) goto done;

    /* Still processing parent groups */
    ret = EAGAIN;

done:
    talloc_free(tmp_ctx);
    return ret;
}