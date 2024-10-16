static void connection_reset(connection *con) {
	request_st * const r = &con->request;
	request_reset(r);
	config_reset_config(r);
	r->bytes_read_ckpt = 0;
	r->bytes_written_ckpt = 0;
	con->is_readable = 1;

	con->bytes_written = 0;
	con->bytes_written_cur_second = 0;
	con->bytes_read = 0;
}