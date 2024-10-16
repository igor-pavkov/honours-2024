void comm_handle(const struct iface_port *ifp)
{
	static struct msg_digest *md;

#if defined(IP_RECVERR) && defined(MSG_ERRQUEUE)
	/* Even though select(2) says that there is a message,
	 * it might only be a MSG_ERRQUEUE message.  At least
	 * sometimes that leads to a hanging recvfrom.  To avoid
	 * what appears to be a kernel bug, check_msg_errqueue
	 * uses poll(2) and tells us if there is anything for us
	 * to read.
	 *
	 * This is early enough that teardown isn't required:
	 * just return on failure.
	 */
	if (!check_msg_errqueue(ifp, POLLIN))
		return; /* no normal message to read */

#endif /* defined(IP_RECVERR) && defined(MSG_ERRQUEUE) */

	md = alloc_md();
	md->iface = ifp;

	if (read_packet(md))
		process_packet(&md);

	if (md != NULL)
		release_md(md);

	cur_state = NULL;
	reset_cur_connection();
	cur_from = NULL;
}