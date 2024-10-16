static struct avrcp *find_session(GSList *list, struct btd_device *dev)
{
	for (; list; list = list->next) {
		struct avrcp *session = list->data;

		if (session->dev == dev)
			return session;
	}

	return NULL;
}