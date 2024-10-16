static unsigned int cpuset_mems_nr(unsigned int *array)
{
	int node;
	unsigned int nr = 0;

	for_each_node_mask(node, cpuset_current_mems_allowed)
		nr += array[node];

	return nr;
}