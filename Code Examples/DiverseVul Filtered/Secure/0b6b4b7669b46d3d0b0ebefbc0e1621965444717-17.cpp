static void sdap_get_initgr_done(struct tevent_req *subreq)
{
    struct tevent_req *req = tevent_req_callback_data(subreq,
                                                      struct tevent_req);
    struct sdap_get_initgr_state *state = tevent_req_data(req,
                                               struct sdap_get_initgr_state);
    int ret;
    TALLOC_CTX *tmp_ctx;
    gid_t primary_gid;
    char *gid;
    char *sid_str;
    char *dom_sid_str;
    char *group_sid_str;
    struct sdap_options *opts = state->opts;

    DEBUG(SSSDBG_TRACE_ALL, "Initgroups done\n");

    tmp_ctx = talloc_new(NULL);
    if (!tmp_ctx) {
        tevent_req_error(req, ENOMEM);
        return;
    }

    switch (state->opts->schema_type) {
    case SDAP_SCHEMA_RFC2307:
        ret = sdap_initgr_rfc2307_recv(subreq);
        break;

    case SDAP_SCHEMA_RFC2307BIS:
    case SDAP_SCHEMA_AD:
        if (state->opts->dc_functional_level >= DS_BEHAVIOR_WIN2003
            && dp_opt_get_bool(state->opts->basic, SDAP_AD_USE_TOKENGROUPS)) {

            ret = sdap_ad_tokengroups_initgroups_recv(subreq);
        }
        else if (state->opts->support_matching_rule
                && dp_opt_get_bool(state->opts->basic,
                                   SDAP_AD_MATCHING_RULE_INITGROUPS)) {
            ret = sdap_get_ad_match_rule_initgroups_recv(subreq);
        } else {
            ret = sdap_initgr_rfc2307bis_recv(subreq);
        }
        break;

    case SDAP_SCHEMA_IPA_V1:
        ret = sdap_initgr_nested_recv(subreq);
        break;

    default:

        ret = EINVAL;
        break;
    }

    talloc_zfree(subreq);
    if (ret) {
        DEBUG(SSSDBG_TRACE_ALL, "Error in initgroups: [%d][%s]\n",
                  ret, strerror(ret));
        goto fail;
    }

    /* We also need to update the user's primary group, since
     * the user may not be an explicit member of that group
     */

    if (state->use_id_mapping) {
        DEBUG(SSSDBG_TRACE_LIBS,
              "Mapping primary group to unix ID\n");

        /* The primary group ID is just the RID part of the objectSID
         * of the group. Generate the GID by adding this to the domain
         * SID value.
         */

        /* Get the user SID so we can extract the domain SID
         * from it.
         */
        ret = sdap_attrs_get_sid_str(
                tmp_ctx, opts->idmap_ctx, state->orig_user,
                opts->user_map[SDAP_AT_USER_OBJECTSID].sys_name,
                &sid_str);
        if (ret != EOK) goto fail;

        /* Get the domain SID from the user SID */
        ret = sdap_idmap_get_dom_sid_from_object(tmp_ctx, sid_str,
                                                 &dom_sid_str);
        if (ret != EOK) {
            DEBUG(SSSDBG_MINOR_FAILURE,
                  "Could not parse domain SID from [%s]\n", sid_str);
            goto fail;
        }

        ret = sysdb_attrs_get_uint32_t(
                state->orig_user,
                opts->user_map[SDAP_AT_USER_PRIMARY_GROUP].sys_name,
                &primary_gid);
        if (ret != EOK) {
            DEBUG(SSSDBG_MINOR_FAILURE,
                  "no primary group ID provided\n");
            ret = EINVAL;
            goto fail;
        }

        /* Add the RID to the end */
        group_sid_str = talloc_asprintf(tmp_ctx, "%s-%lu",
                                        dom_sid_str,
                                        (unsigned long)primary_gid);
        if (!group_sid_str) {
            ret = ENOMEM;
            goto fail;
        }

        /* Convert the SID into a UNIX group ID */
        ret = sdap_idmap_sid_to_unix(opts->idmap_ctx, group_sid_str,
                                     &primary_gid);
        if (ret != EOK) goto fail;
    } else {
        ret = sysdb_attrs_get_uint32_t(state->orig_user, SYSDB_GIDNUM,
                                       &primary_gid);
        if (ret != EOK) {
            DEBUG(SSSDBG_TRACE_FUNC, "Could not find user's primary GID\n");
            goto fail;
        }
    }

    gid = talloc_asprintf(state, "%lu", (unsigned long)primary_gid);
    if (gid == NULL) {
        ret = ENOMEM;
        goto fail;
    }

    subreq = groups_get_send(req, state->ev, state->id_ctx,
                             state->id_ctx->opts->sdom, state->conn,
                             gid, BE_FILTER_IDNUM, BE_ATTR_ALL, NULL);
    if (!subreq) {
        ret = ENOMEM;
        goto fail;
    }
    tevent_req_set_callback(subreq, sdap_get_initgr_pgid, req);

    talloc_free(tmp_ctx);
    return;

fail:
    talloc_free(tmp_ctx);
    tevent_req_error(req, ret);
    return;
}