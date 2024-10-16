static void sdap_initgr_rfc2307bis_done(struct tevent_req *subreq)
{
    errno_t ret;
    struct tevent_req *req =
            tevent_req_callback_data(subreq, struct tevent_req);
    struct sdap_initgr_rfc2307bis_state *state =
            tevent_req_data(req, struct sdap_initgr_rfc2307bis_state);
    bool in_transaction = false;
    errno_t tret;

    ret = rfc2307bis_nested_groups_recv(subreq);
    talloc_zfree(subreq);
    if (ret != EOK) {
        tevent_req_error(req, ret);
        return;
    }

    ret = sysdb_transaction_start(state->sysdb);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Failed to start transaction\n");
        goto fail;
    }
    in_transaction = true;

    /* save the groups if they are not cached */
    ret = save_rfc2307bis_groups(state);
    if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE,
              "Could not save groups memberships [%d]", ret);
        goto fail;
    }

    /* save the group membership */
    ret = save_rfc2307bis_group_memberships(state);
    if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE,
              "Could not save group memberships [%d]", ret);
        goto fail;
    }

    /* save the user memberships */
    ret = save_rfc2307bis_user_memberships(state);
    if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE,
              "Could not save user memberships [%d]", ret);
        goto fail;
    }

    ret = sysdb_transaction_commit(state->sysdb);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Failed to commit transaction\n");
        goto fail;
    }
    in_transaction = false;

    tevent_req_done(req);
    return;

fail:
    if (in_transaction) {
        tret = sysdb_transaction_cancel(state->sysdb);
        if (tret != EOK) {
            DEBUG(SSSDBG_CRIT_FAILURE, "Failed to cancel transaction\n");
        }
    }
    tevent_req_error(req, ret);
    return;
}