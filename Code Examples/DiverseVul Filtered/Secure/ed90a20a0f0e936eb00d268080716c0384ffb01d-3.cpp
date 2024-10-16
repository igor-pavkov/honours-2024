static void sudo_dp_reconnect_init(struct sbus_connection *conn,
                                   int status,
                                   void *pvt)
{
    struct be_conn *be_conn = talloc_get_type(pvt, struct be_conn);
    int ret;

    /* Did we reconnect successfully? */
    if (status == SBUS_RECONNECT_SUCCESS) {
        DEBUG(SSSDBG_TRACE_FUNC, "Reconnected to the Data Provider.\n");

        /* Identify ourselves to the data provider */
        ret = rdp_register_client(be_conn, "SUDO");
        /* all fine */
        if (ret == EOK) {
            handle_requests_after_reconnect(be_conn->rctx);
            return;
        }
    }

    /* Failed to reconnect */
    DEBUG(SSSDBG_FATAL_FAILURE, "Could not reconnect to %s provider.\n",
                                 be_conn->domain->name);
}