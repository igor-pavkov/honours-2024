void notify_fname(connection_struct *conn, uint32_t action, uint32_t filter,
		  const char *path)
{
	struct notify_context *notify_ctx = conn->sconn->notify_ctx;

	if (path[0] == '.' && path[1] == '/') {
		path += 2;
	}

	notify_trigger(notify_ctx, action, filter, conn->connectpath, path);
}