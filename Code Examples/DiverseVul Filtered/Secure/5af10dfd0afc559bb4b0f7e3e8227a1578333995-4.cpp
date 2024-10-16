int hugetlb_report_node_meminfo(int nid, char *buf)
{
	struct hstate *h = &default_hstate;
	if (!hugepages_supported())
		return 0;
	return sprintf(buf,
		"Node %d HugePages_Total: %5u\n"
		"Node %d HugePages_Free:  %5u\n"
		"Node %d HugePages_Surp:  %5u\n",
		nid, h->nr_huge_pages_node[nid],
		nid, h->free_huge_pages_node[nid],
		nid, h->surplus_huge_pages_node[nid]);
}