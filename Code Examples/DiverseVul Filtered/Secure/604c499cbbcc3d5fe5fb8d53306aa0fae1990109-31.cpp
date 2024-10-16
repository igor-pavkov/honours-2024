static struct persistent_gnt *get_persistent_gnt(struct xen_blkif *blkif,
						 grant_ref_t gref)
{
	struct persistent_gnt *data;
	struct rb_node *node = NULL;

	node = blkif->persistent_gnts.rb_node;
	while (node) {
		data = container_of(node, struct persistent_gnt, node);

		if (gref < data->gnt)
			node = node->rb_left;
		else if (gref > data->gnt)
			node = node->rb_right;
		else {
			if(test_bit(PERSISTENT_GNT_ACTIVE, data->flags)) {
				pr_alert_ratelimited(DRV_PFX " requesting a grant already in use\n");
				return NULL;
			}
			set_bit(PERSISTENT_GNT_ACTIVE, data->flags);
			atomic_inc(&blkif->persistent_gnt_in_use);
			return data;
		}
	}
	return NULL;
}