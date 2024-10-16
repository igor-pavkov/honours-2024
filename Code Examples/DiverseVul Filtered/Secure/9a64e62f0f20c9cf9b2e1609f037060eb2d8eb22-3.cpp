static int apply_proxy_config(http_subtransport *t)
{
	int error;
	git_proxy_t proxy_type;

	if (!git_stream_supports_proxy(t->io))
		return 0;

	proxy_type = t->owner->proxy.type;

	if (proxy_type == GIT_PROXY_NONE)
		return 0;

	if (proxy_type == GIT_PROXY_AUTO) {
		char *url;
		git_proxy_options opts = GIT_PROXY_OPTIONS_INIT;

		if ((error = git_remote__get_http_proxy(t->owner->owner, !!t->connection_data.use_ssl, &url)) < 0)
			return error;

		opts.type = GIT_PROXY_SPECIFIED;
		opts.url = url;
		error = git_stream_set_proxy(t->io, &opts);
		git__free(url);

		return error;
	}

	return git_stream_set_proxy(t->io, &t->owner->proxy);
}