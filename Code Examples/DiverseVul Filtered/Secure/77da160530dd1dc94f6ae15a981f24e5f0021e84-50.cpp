struct gendisk *alloc_disk(int minors)
{
	return alloc_disk_node(minors, NUMA_NO_NODE);
}