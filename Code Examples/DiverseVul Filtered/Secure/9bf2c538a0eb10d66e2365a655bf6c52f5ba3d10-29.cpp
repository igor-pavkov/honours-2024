static struct nsid_cache *netns_map_get_by_nsid(int nsid)
{
	uint32_t h = NSID_HASH_NSID(nsid);
	struct hlist_node *n;

	hlist_for_each(n, &nsid_head[h]) {
		struct nsid_cache *c = container_of(n, struct nsid_cache,
						    nsid_hash);
		if (c->nsid == nsid)
			return c;
	}

	return NULL;
}