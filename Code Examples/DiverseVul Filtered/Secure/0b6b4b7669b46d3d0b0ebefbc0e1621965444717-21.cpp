static void rfc2307bis_nested_groups_done(struct tevent_req *subreq)
{
    errno_t ret;
    struct tevent_req *req =
            tevent_req_callback_data(subreq, struct tevent_req);
    struct sdap_rfc2307bis_nested_ctx *state =
            tevent_req_data(req, struct sdap_rfc2307bis_nested_ctx);

    ret = rfc2307bis_nested_groups_recv(subreq);
    talloc_zfree(subreq);
    if (ret != EOK) {
        DEBUG(SSSDBG_TRACE_FUNC, "rfc2307bis_nested failed [%d][%s]\n",
                  ret, strerror(ret));
        tevent_req_error(req, ret);
        return;
    }

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
        return;
    }
}