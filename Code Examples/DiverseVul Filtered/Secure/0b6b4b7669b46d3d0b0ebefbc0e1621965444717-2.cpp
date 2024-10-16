static void sdap_get_initgr_user(struct tevent_req *subreq)
{
    struct tevent_req *req = tevent_req_callback_data(subreq,
                                                      struct tevent_req);
    struct sdap_get_initgr_state *state = tevent_req_data(req,
                                               struct sdap_get_initgr_state);
    struct sysdb_attrs **usr_attrs;
    size_t count;
    int ret;
    errno_t sret;
    const char *orig_dn;
    const char *cname;
    bool in_transaction = false;
    char *expected_basedn;
    size_t expected_basedn_len;
    size_t dn_len;
    size_t c = 0;

    DEBUG(SSSDBG_TRACE_ALL, "Receiving info for the user\n");

    ret = sdap_get_generic_recv(subreq, state, &count, &usr_attrs);
    talloc_zfree(subreq);
    if (ret) {
        tevent_req_error(req, ret);
        return;
    }

    if (count == 0) {
        /* No users found in this search */
        state->user_base_iter++;
        if (state->user_search_bases[state->user_base_iter]) {
            /* There are more search bases to try */
            ret = sdap_get_initgr_next_base(req);
            if (ret != EOK) {
                tevent_req_error(req, ret);
            }
            return;
        }

        /* fallback to fetch a local user if required */
        if ((state->opts->schema_type == SDAP_SCHEMA_RFC2307) &&
            (dp_opt_get_bool(state->opts->basic,
                             SDAP_RFC2307_FALLBACK_TO_LOCAL_USERS) == true)) {
            ret = sdap_fallback_local_user(state, state->opts,
                                           state->name, -1, &usr_attrs);
        } else {
            ret = ENOENT;
        }

        if (ret != EOK) {
            tevent_req_error(req, ret);
            return;
        }
    } else if (count != 1) {
        DEBUG(SSSDBG_OP_FAILURE,
              "Expected one user entry and got %zu\n", count);

        ret = domain_to_basedn(state, state->dom->name, &expected_basedn);
        if (ret != EOK) {
            DEBUG(SSSDBG_OP_FAILURE, "domain_to_basedn failed.\n");
            tevent_req_error(req, ret);
            return;
        }
        expected_basedn = talloc_asprintf(state, "%s%s",
                                                 "cn=users,", expected_basedn);
        if (expected_basedn == NULL) {
            DEBUG(SSSDBG_OP_FAILURE, "talloc_append failed.\n");
            tevent_req_error(req, ENOMEM);
            return;
        }

        DEBUG(SSSDBG_TRACE_ALL, "Expected BaseDN is [%s].\n", expected_basedn);
        expected_basedn_len = strlen(expected_basedn);

        for (c = 0; c < count; c++) {
            ret = sysdb_attrs_get_string(usr_attrs[c], SYSDB_ORIG_DN, &orig_dn);
            if (ret != EOK) {
                DEBUG(SSSDBG_OP_FAILURE, "sysdb_attrs_get_string failed.\n");
                tevent_req_error(req, ret);
                return;
            }
            dn_len = strlen(orig_dn);

            if (dn_len > expected_basedn_len
                    && strcasecmp(orig_dn + (dn_len - expected_basedn_len),
                                  expected_basedn) == 0) {
                DEBUG(SSSDBG_TRACE_ALL,
                      "Found matching dn [%s].\n", orig_dn);
                break;
            }
        }

        if (c == count) {
            DEBUG(SSSDBG_OP_FAILURE, "No matching DN found.\n");
            tevent_req_error(req, EINVAL);
            return;
        }
    }

    state->orig_user = usr_attrs[c];

    ret = sysdb_transaction_start(state->sysdb);
    if (ret) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Failed to start transaction\n");
        goto fail;
    }
    in_transaction = true;

    DEBUG(SSSDBG_TRACE_ALL, "Storing the user\n");

    ret = sdap_save_user(state, state->sysdb,
                         state->opts, state->dom,
                         state->orig_user,
                         true, NULL, 0);
    if (ret) {
        goto fail;
    }

    DEBUG(SSSDBG_TRACE_ALL, "Commit change\n");

    ret = sysdb_transaction_commit(state->sysdb);
    if (ret) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Failed to commit transaction\n");
        goto fail;
    }
    in_transaction = false;

    ret = sysdb_get_real_name(state, state->sysdb,
                              state->dom, state->name, &cname);
    if (ret != EOK) {
        DEBUG(SSSDBG_OP_FAILURE, "Cannot canonicalize username\n");
        tevent_req_error(req, ret);
        return;
    }

    DEBUG(SSSDBG_TRACE_ALL, "Process user's groups\n");

    switch (state->opts->schema_type) {
    case SDAP_SCHEMA_RFC2307:
        subreq = sdap_initgr_rfc2307_send(state, state->ev, state->opts,
                                          state->sysdb, state->dom, state->sh,
                                          cname);
        if (!subreq) {
            tevent_req_error(req, ENOMEM);
            return;
        }
        tevent_req_set_callback(subreq, sdap_get_initgr_done, req);
        break;

    case SDAP_SCHEMA_RFC2307BIS:
    case SDAP_SCHEMA_AD:
        ret = sysdb_attrs_get_string(state->orig_user,
                                     SYSDB_ORIG_DN,
                                     &orig_dn);
        if (ret != EOK) {
            tevent_req_error(req, ret);
            return;
        }

        if (state->opts->dc_functional_level >= DS_BEHAVIOR_WIN2003
            && dp_opt_get_bool(state->opts->basic, SDAP_AD_USE_TOKENGROUPS)) {
            /* Take advantage of AD's tokenGroups mechanism to look up all
             * parent groups in a single request.
             */
            subreq = sdap_ad_tokengroups_initgroups_send(state, state->ev,
                                                         state->id_ctx,
                                                         state->conn,
                                                         state->opts,
                                                         state->sysdb,
                                                         state->dom,
                                                         state->sh,
                                                         cname, orig_dn,
                                                         state->timeout,
                                                         state->use_id_mapping);
        } else if (state->opts->support_matching_rule
                    && dp_opt_get_bool(state->opts->basic,
                                       SDAP_AD_MATCHING_RULE_INITGROUPS)) {
            /* Take advantage of AD's extensibleMatch filter to look up
             * all parent groups in a single request.
             */
            subreq = sdap_get_ad_match_rule_initgroups_send(state, state->ev,
                                                            state->opts,
                                                            state->sysdb,
                                                            state->dom,
                                                            state->sh,
                                                            cname, orig_dn,
                                                            state->timeout);
        } else {
            subreq = sdap_initgr_rfc2307bis_send(
                    state, state->ev, state->opts,
                    state->sdom, state->sh,
                    cname, orig_dn);
        }
        if (!subreq) {
            tevent_req_error(req, ENOMEM);
            return;
        }

        talloc_steal(subreq, orig_dn);
        tevent_req_set_callback(subreq, sdap_get_initgr_done, req);
        break;

    case SDAP_SCHEMA_IPA_V1:
        subreq = sdap_initgr_nested_send(state, state->ev, state->opts,
                                         state->sysdb, state->dom, state->sh,
                                         state->orig_user, state->grp_attrs);
        if (!subreq) {
            tevent_req_error(req, ENOMEM);
            return;
        }
        tevent_req_set_callback(subreq, sdap_get_initgr_done, req);
        return;

    default:
        tevent_req_error(req, EINVAL);
        return;
    }

    return;
fail:
    if (in_transaction) {
        sret = sysdb_transaction_cancel(state->sysdb);
        if (sret != EOK) {
            DEBUG(SSSDBG_CRIT_FAILURE, "Failed to cancel transaction\n");
        }
    }
    tevent_req_error(req, ret);
}