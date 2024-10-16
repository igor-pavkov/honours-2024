static void rfc2307bis_nested_groups_process(struct tevent_req *subreq)
{
    errno_t ret;
    struct tevent_req *req =
            tevent_req_callback_data(subreq, struct tevent_req);
    struct sdap_rfc2307bis_nested_ctx *state =
            tevent_req_data(req, struct sdap_rfc2307bis_nested_ctx);
    size_t count;
    size_t i;
    struct sysdb_attrs **ldap_groups;
    struct sdap_nested_group *ngr;
    hash_value_t value;
    hash_key_t key;
    int hret;

    ret = sdap_get_generic_recv(subreq, state,
                                &count,
                                &ldap_groups);
    talloc_zfree(subreq);
    if (ret) {
        tevent_req_error(req, ret);
        return;
    }

    DEBUG(SSSDBG_TRACE_LIBS,
          "Found %zu parent groups of [%s]\n", count, state->orig_dn);
    ngr = state->processed_groups[state->group_iter];

    /* Add this batch of groups to the list */
    if (count > 0) {
        ngr->ldap_parents =
                talloc_realloc(ngr,
                               ngr->ldap_parents,
                               struct sysdb_attrs *,
                               ngr->parents_count + count + 1);
        if (!ngr->ldap_parents) {
            tevent_req_error(req, ENOMEM);
            return;
        }

        /* Copy the new groups into the list.
         * They're allocated on 'state' so we need to move them
         * onto ldap_parents so that the data won't disappear when
         * we finish this nesting level.
         */
        for (i = 0; i < count; i++) {
            ngr->ldap_parents[ngr->parents_count + i] =
                talloc_steal(ngr->ldap_parents, ldap_groups[i]);
        }

        ngr->parents_count += count;

        ngr->ldap_parents[ngr->parents_count] = NULL;
        DEBUG(SSSDBG_TRACE_INTERNAL,
              "Total of %zu direct parents after this iteration\n",
               ngr->parents_count);
    }

    state->base_iter++;

    /* Check for additional search bases, and iterate
     * through again.
     */
    if (state->search_bases[state->base_iter] != NULL) {
        ret = rfc2307bis_nested_groups_next_base(req);
        if (ret != EOK) {
            tevent_req_error(req, ret);
        }
        return;
    }

    /* Reset the base iterator for future lookups */
    state->base_iter = 0;

    /* Save the group into the hash table */
    key.type = HASH_KEY_STRING;
    key.str = talloc_strdup(state, state->primary_name);
    if (!key.str) {
        tevent_req_error(req, ENOMEM);
        return;
    }

    /* Steal the nested group entry on the group_hash context so it can
     * outlive this request */
    talloc_steal(state->group_hash, ngr);

    value.type = HASH_VALUE_PTR;
    value.ptr = ngr;

    hret = hash_enter(state->group_hash, &key, &value);
    if (hret != HASH_SUCCESS) {
        talloc_free(key.str);
        tevent_req_error(req, EIO);
        return;
    }
    talloc_free(key.str);

    if (ngr->parents_count == 0) {
        /* No parent groups for this group in LDAP
         * Move on to the next group
         */
        state->group_iter++;
        while (state->group_iter < state->num_groups) {
            ret = rfc2307bis_nested_groups_step(req);
            if (ret == EAGAIN) {
                /* Looking up parent groups.. */
                return;
            } else if (ret != EOK) {
                tevent_req_error(req, ret);
                return;
            }

            /* EOK means this group has already been processed
             * in another nesting level */
            state->group_iter++;
        }

        if (state->group_iter == state->num_groups) {
            /* All groups processed. Done. */
            tevent_req_done(req);
        }
        return;
    }

    /* Otherwise, recurse into the groups */
    subreq = rfc2307bis_nested_groups_send(
            state, state->ev, state->opts, state->sysdb,
            state->dom, state->sh,
            ngr->ldap_parents,
            ngr->parents_count,
            state->group_hash,
            state->nesting_level+1);
    if (!subreq) {
        tevent_req_error(req, EIO);
        return;
    }
    tevent_req_set_callback(subreq, rfc2307bis_nested_groups_done, req);
}