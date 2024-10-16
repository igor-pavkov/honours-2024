void shut_down(int code)
{
    int i;

    in_shutdown = 1;

    proc_cleanup();

    /* close local mailbox */
    if (group_state)
	index_close(&group_state);

    /* close backend connections */
    i = 0;
    while (backend_cached && backend_cached[i]) {
	proxy_downserver(backend_cached[i]);
	free(backend_cached[i]->context);
	free(backend_cached[i]);
	i++;
    }
    if (backend_cached) free(backend_cached);

    sync_log_done();

    duplicate_done();

    mboxlist_close();
    mboxlist_done();

    quotadb_close();
    quotadb_done();

    denydb_close();
    denydb_done();

    annotatemore_close();
    annotatemore_done();

    if (nntp_in) {
	prot_NONBLOCK(nntp_in);
	prot_fill(nntp_in);
	prot_free(nntp_in);
    }

    if (nntp_out) {
	prot_flush(nntp_out);
	prot_free(nntp_out);
    }

    if (protin) protgroup_free(protin);

#ifdef HAVE_SSL
    tls_shutdown_serverengine();
#endif

    if (newsgroups) free_wildmats(newsgroups);

    cyrus_done();

    exit(code);
}