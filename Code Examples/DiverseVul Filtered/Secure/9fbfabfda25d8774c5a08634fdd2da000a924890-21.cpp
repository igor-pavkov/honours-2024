static void free_partitions(struct parsed_partitions *state)
{
	vfree(state->parts);
	kfree(state);
}