static void sdap_initgr_nested_store(struct tevent_req *req)
{
    errno_t ret;
    struct sdap_initgr_nested_state *state;
    bool in_transaction = false;
    errno_t tret;

    state = tevent_req_data(req, struct sdap_initgr_nested_state);

    ret = sysdb_transaction_start(state->sysdb);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Failed to start transaction\n");
        goto fail;
    }
    in_transaction = true;

    /* save the groups if they are not already */
    ret = sdap_initgr_store_groups(state);
    if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE, "Could not save groups [%d]: %s\n",
                  ret, strerror(ret));
        goto fail;
    }

    /* save the group memberships */
    ret = sdap_initgr_store_group_memberships(state);
    if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE,
              "Could not save group memberships [%d]: %s\n",
                  ret, strerror(ret));
        goto fail;
    }

    /* save the user memberships */
    ret = sdap_initgr_store_user_memberships(state);
    if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE,
              "Could not save user memberships [%d]: %s\n",
                  ret, strerror(ret));
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