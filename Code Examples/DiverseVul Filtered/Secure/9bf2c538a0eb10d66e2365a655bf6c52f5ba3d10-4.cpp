static void netns_map_del(struct nsid_cache *c)
{
	hlist_del(&c->name_hash);
	hlist_del(&c->nsid_hash);
	free(c);
}