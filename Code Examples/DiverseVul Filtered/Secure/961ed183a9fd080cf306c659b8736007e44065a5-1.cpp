static void *clusterip_seq_start(struct seq_file *s, loff_t *pos)
{
	struct clusterip_config *c = s->private;
	unsigned int weight;
	u_int32_t local_nodes;
	struct clusterip_seq_position *idx;

	/* FIXME: possible race */
	local_nodes = c->local_nodes;
	weight = hweight32(local_nodes);
	if (*pos >= weight)
		return NULL;

	idx = kmalloc(sizeof(struct clusterip_seq_position), GFP_KERNEL);
	if (!idx)
		return ERR_PTR(-ENOMEM);

	idx->pos = *pos;
	idx->weight = weight;
	idx->bit = ffs(local_nodes);
	idx->val = local_nodes;
	clear_bit(idx->bit - 1, &idx->val);

	return idx;
}