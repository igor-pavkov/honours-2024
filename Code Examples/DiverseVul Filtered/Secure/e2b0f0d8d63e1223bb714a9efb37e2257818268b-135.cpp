static struct avrcp_server *find_server(GSList *list, struct btd_adapter *a)
{
	for (; list; list = list->next) {
		struct avrcp_server *server = list->data;

		if (server->adapter == a)
			return server;
	}

	return NULL;
}