static int build_dma_sg(const struct sk_buff *skb, struct urb *urb)
{
	unsigned num_sgs, total_len = 0;
	int i, s = 0;

	num_sgs = skb_shinfo(skb)->nr_frags + 1;
	if (num_sgs == 1)
		return 0;

	/* reserve one for zero packet */
	urb->sg = kmalloc((num_sgs + 1) * sizeof(struct scatterlist),
			  GFP_ATOMIC);
	if (!urb->sg)
		return -ENOMEM;

	urb->num_sgs = num_sgs;
	sg_init_table(urb->sg, urb->num_sgs + 1);

	sg_set_buf(&urb->sg[s++], skb->data, skb_headlen(skb));
	total_len += skb_headlen(skb);

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		struct skb_frag_struct *f = &skb_shinfo(skb)->frags[i];

		total_len += skb_frag_size(f);
		sg_set_page(&urb->sg[i + s], f->page.p, f->size,
				f->page_offset);
	}
	urb->transfer_buffer_length = total_len;

	return 1;
}