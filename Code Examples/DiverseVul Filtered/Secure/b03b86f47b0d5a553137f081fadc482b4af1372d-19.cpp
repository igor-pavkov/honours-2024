static void connection_handle_close_state(connection *con) {
	connection_read_for_eos(con);

	if (log_epoch_secs - con->close_timeout_ts > HTTP_LINGER_TIMEOUT) {
		connection_close(con);
	}
}