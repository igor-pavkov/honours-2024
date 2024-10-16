char *get_name_from_nsid(int nsid)
{
	struct nsid_cache *c;

	netns_nsid_socket_init();
	netns_map_init();

	c = netns_map_get_by_nsid(nsid);
	if (c)
		return c->name;

	return NULL;
}