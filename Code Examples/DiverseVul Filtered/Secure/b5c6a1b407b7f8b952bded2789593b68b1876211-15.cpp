static int http_action(
	git_smart_subtransport_stream **stream,
	git_smart_subtransport *subtransport,
	const char *url,
	git_smart_service_t action)
{
	http_subtransport *t = (http_subtransport *)subtransport;
	int ret;

	if (!stream)
		return -1;

	if ((!t->connection_data.host || !t->connection_data.port || !t->connection_data.path) &&
		 (ret = gitno_connection_data_from_url(&t->connection_data, url, NULL)) < 0)
		return ret;

	if ((ret = http_connect(t)) < 0)
		return ret;

	switch (action) {
	case GIT_SERVICE_UPLOADPACK_LS:
		return http_uploadpack_ls(t, stream);

	case GIT_SERVICE_UPLOADPACK:
		return http_uploadpack(t, stream);

	case GIT_SERVICE_RECEIVEPACK_LS:
		return http_receivepack_ls(t, stream);

	case GIT_SERVICE_RECEIVEPACK:
		return http_receivepack(t, stream);
	}

	*stream = NULL;
	return -1;
}