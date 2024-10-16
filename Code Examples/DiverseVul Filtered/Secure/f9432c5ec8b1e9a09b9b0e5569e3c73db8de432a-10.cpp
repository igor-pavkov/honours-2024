static struct sk_buff *rfcomm_wmalloc(struct rfcomm_dev *dev, unsigned long size, gfp_t priority)
{
	if (atomic_read(&dev->wmem_alloc) < rfcomm_room(dev->dlc)) {
		struct sk_buff *skb = alloc_skb(size, priority);
		if (skb) {
			rfcomm_set_owner_w(skb, dev);
			return skb;
		}
	}
	return NULL;
}