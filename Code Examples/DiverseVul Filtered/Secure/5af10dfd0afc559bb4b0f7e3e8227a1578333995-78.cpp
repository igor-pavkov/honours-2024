static struct page *dequeue_huge_page_nodemask(struct hstate *h, gfp_t gfp_mask, int nid,
		nodemask_t *nmask)
{
	unsigned int cpuset_mems_cookie;
	struct zonelist *zonelist;
	struct zone *zone;
	struct zoneref *z;
	int node = -1;

	zonelist = node_zonelist(nid, gfp_mask);

retry_cpuset:
	cpuset_mems_cookie = read_mems_allowed_begin();
	for_each_zone_zonelist_nodemask(zone, z, zonelist, gfp_zone(gfp_mask), nmask) {
		struct page *page;

		if (!cpuset_zone_allowed(zone, gfp_mask))
			continue;
		/*
		 * no need to ask again on the same node. Pool is node rather than
		 * zone aware
		 */
		if (zone_to_nid(zone) == node)
			continue;
		node = zone_to_nid(zone);

		page = dequeue_huge_page_node_exact(h, node);
		if (page)
			return page;
	}
	if (unlikely(read_mems_allowed_retry(cpuset_mems_cookie)))
		goto retry_cpuset;

	return NULL;
}