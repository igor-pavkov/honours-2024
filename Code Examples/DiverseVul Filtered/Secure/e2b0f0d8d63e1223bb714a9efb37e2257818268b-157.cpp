static void destroy_browsing(void *data)
{
	struct avrcp *session = data;

	session->browsing_id = 0;
}