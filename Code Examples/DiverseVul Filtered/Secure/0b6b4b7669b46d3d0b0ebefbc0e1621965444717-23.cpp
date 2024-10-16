static struct tevent_req *sdap_initgr_nested_send(TALLOC_CTX *memctx,
                                                  struct tevent_context *ev,
                                                  struct sdap_options *opts,
                                                  struct sysdb_ctx *sysdb,
                                                  struct sss_domain_info *dom,
                                                  struct sdap_handle *sh,
                                                  struct sysdb_attrs *user,
                                                  const char **grp_attrs)
{
    struct tevent_req *req;
    struct sdap_initgr_nested_state *state;
    errno_t ret;
    int deref_threshold;

    req = tevent_req_create(memctx, &state, struct sdap_initgr_nested_state);
    if (!req) return NULL;

    state->ev = ev;
    state->opts = opts;
    state->sysdb = sysdb;
    state->dom = dom;
    state->sh = sh;
    state->grp_attrs = grp_attrs;
    state->user = user;
    state->op = NULL;

    ret = sdap_get_user_primary_name(memctx, opts, user, dom, &state->username);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "User entry had no username\n");
        goto immediate;
    }

    ret = sysdb_attrs_get_el(state->user, SYSDB_MEMBEROF, &state->memberof);
    if (ret || !state->memberof || state->memberof->num_values == 0) {
        DEBUG(SSSDBG_CONF_SETTINGS, "User entry lacks original memberof ?\n");
        /* We can't find any groups for this user, so we'll
         * have to assume there aren't any. Just return
         * success here.
         */
        ret = EOK;
        goto immediate;
    }

    state->groups = talloc_zero_array(state, struct sysdb_attrs *,
                                      state->memberof->num_values + 1);;
    if (!state->groups) {
        ret = ENOMEM;
        goto immediate;
    }
    state->groups_cur = 0;

    deref_threshold = dp_opt_get_int(state->opts->basic,
                                     SDAP_DEREF_THRESHOLD);
    if (sdap_has_deref_support(state->sh, state->opts) &&
        deref_threshold < state->memberof->num_values) {
        ret = sysdb_attrs_get_string(user, SYSDB_ORIG_DN,
                                     &state->orig_dn);
        if (ret != EOK) goto immediate;

        ret = sdap_initgr_nested_deref_search(req);
        if (ret != EAGAIN) goto immediate;
    } else {
        ret = sdap_initgr_nested_noderef_search(req);
        if (ret != EAGAIN) goto immediate;
    }

    return req;

immediate:
    if (ret == EOK) {
        tevent_req_done(req);
    } else {
        tevent_req_error(req, ret);
    }
    tevent_req_post(req, ev);
    return req;
}