static int netns_map_add(int nsid, const char *name)
{
	struct nsid_cache *c;
	uint32_t h;

	if (netns_map_get_by_nsid(nsid) != NULL)
		return -EEXIST;

	c = malloc(sizeof(*c) + strlen(name) + 1);
	if (c == NULL) {
		perror("malloc");
		return -ENOMEM;
	}
	c->nsid = nsid;
	strcpy(c->name, name);

	h = NSID_HASH_NSID(nsid);
	hlist_add_head(&c->nsid_hash, &nsid_head[h]);

	h = NSID_HASH_NAME(name);
	hlist_add_head(&c->name_hash, &name_head[h]);

	return 0;
}