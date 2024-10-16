static void *clusterip_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct clusterip_seq_position *idx = v;

	*pos = ++idx->pos;
	if (*pos >= idx->weight) {
		kfree(v);
		return NULL;
	}
	idx->bit = ffs(idx->val);
	clear_bit(idx->bit - 1, &idx->val);
	return idx;
}