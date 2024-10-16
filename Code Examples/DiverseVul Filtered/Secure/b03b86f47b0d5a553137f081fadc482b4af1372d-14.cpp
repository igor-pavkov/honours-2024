static void connection_read_for_eos_ssl(connection * const con) {
	if (con->network_read(con, con->read_queue, MAX_READ_LIMIT) < 0)
		con->close_timeout_ts = log_epoch_secs - (HTTP_LINGER_TIMEOUT+1);
	chunkqueue_reset(con->read_queue);
}