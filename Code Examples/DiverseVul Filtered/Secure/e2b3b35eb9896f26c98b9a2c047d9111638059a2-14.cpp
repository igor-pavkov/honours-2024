static int vhost_net_buf_peek_len(void *ptr)
{
	if (tun_is_xdp_buff(ptr)) {
		struct xdp_buff *xdp = tun_ptr_to_xdp(ptr);

		return xdp->data_end - xdp->data;
	}

	return __skb_array_len_with_tag(ptr);
}