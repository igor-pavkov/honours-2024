static int h2c_send_conn_wu(struct h2c *h2c)
{
	int ret = 1;

	if (h2c->rcvd_c <= 0)
		return 1;

	if (!(h2c->flags & H2_CF_WINDOW_OPENED)) {
		/* increase the advertised connection window to 2G on
		 * first update.
		 */
		h2c->flags |= H2_CF_WINDOW_OPENED;
		h2c->rcvd_c += H2_INITIAL_WINDOW_INCREMENT;
	}

	/* send WU for the connection */
	ret = h2c_send_window_update(h2c, 0, h2c->rcvd_c);
	if (ret > 0)
		h2c->rcvd_c = 0;

	return ret;
}