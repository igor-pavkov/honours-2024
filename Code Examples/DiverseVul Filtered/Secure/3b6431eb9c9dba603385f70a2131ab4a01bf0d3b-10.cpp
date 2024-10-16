static void log_error_and_cleanup(char *msg, apr_status_t sts, server_rec *s)
{
    ap_log_error(APLOG_MARK, APLOG_ERR, sts, s, APLOGNO(01760)
                 "%s - all nonce-count checking and one-time nonces "
                 "disabled", msg);

    cleanup_tables(NULL);
}