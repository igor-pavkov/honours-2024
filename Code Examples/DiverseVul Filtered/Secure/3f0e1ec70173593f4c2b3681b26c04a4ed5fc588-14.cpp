static int h2c_handle_ping(struct h2c *h2c)
{
	/* frame length must be exactly 8 */
	if (h2c->dfl != 8) {
		h2c_error(h2c, H2_ERR_FRAME_SIZE_ERROR);
		return 0;
	}

	/* schedule a response */
	if (!(h2c->dff & H2_F_PING_ACK))
		h2c->st0 = H2_CS_FRAME_A;
	return 1;
}