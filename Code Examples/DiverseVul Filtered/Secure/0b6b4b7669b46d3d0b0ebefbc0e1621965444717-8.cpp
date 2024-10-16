static void sdap_initgr_nested_deref_done(struct tevent_req *subreq)
{
    errno_t ret;
    struct tevent_req *req;
    struct sdap_initgr_nested_state *state;
    size_t num_results;
    size_t i;
    struct sdap_deref_attrs **deref_result;

    req = tevent_req_callback_data(subreq, struct tevent_req);
    state = tevent_req_data(req, struct sdap_initgr_nested_state);

    ret = sdap_deref_search_recv(subreq, state,
                                 &num_results,
                                 &deref_result);
    talloc_zfree(subreq);
    if (ret == ENOTSUP) {
        ret = sdap_initgr_nested_noderef_search(req);
        if (ret != EAGAIN) {
            if (ret == EOK) {
                tevent_req_done(req);
            } else {
                tevent_req_error(req, ret);
            }
        }
        return;
    } else if (ret != EOK && ret != ENOENT) {
        tevent_req_error(req, ret);
        return;
    } else if (ret == ENOENT || deref_result == NULL) {
        /* Nothing could be dereferenced. Done. */
        tevent_req_done(req);
        return;
    }

    for (i=0; i < num_results; i++) {
        state->groups[i] = talloc_steal(state->groups,
                                        deref_result[i]->attrs);
    }

    state->groups_cur = num_results;
    sdap_initgr_nested_store(req);
}