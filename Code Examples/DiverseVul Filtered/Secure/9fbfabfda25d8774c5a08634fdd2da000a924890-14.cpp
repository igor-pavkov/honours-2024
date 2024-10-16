static struct parsed_partitions *allocate_partitions(struct gendisk *hd)
{
	struct parsed_partitions *state;
	int nr;

	state = kzalloc(sizeof(*state), GFP_KERNEL);
	if (!state)
		return NULL;

	nr = disk_max_parts(hd);
	state->parts = vzalloc(array_size(nr, sizeof(state->parts[0])));
	if (!state->parts) {
		kfree(state);
		return NULL;
	}

	state->limit = nr;

	return state;
}