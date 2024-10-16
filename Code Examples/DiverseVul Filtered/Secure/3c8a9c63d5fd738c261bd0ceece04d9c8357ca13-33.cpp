static __inline__ ssize_t tun_put_user(struct tun_struct *tun,
				       struct sk_buff *skb,
				       const struct iovec *iv, int len)
{
	struct tun_pi pi = { 0, skb->protocol };
	ssize_t total = 0;

	if (!(tun->flags & TUN_NO_PI)) {
		if ((len -= sizeof(pi)) < 0)
			return -EINVAL;

		if (len < skb->len) {
			/* Packet will be striped */
			pi.flags |= TUN_PKT_STRIP;
		}

		if (memcpy_toiovecend(iv, (void *) &pi, 0, sizeof(pi)))
			return -EFAULT;
		total += sizeof(pi);
	}

	if (tun->flags & TUN_VNET_HDR) {
		struct virtio_net_hdr gso = { 0 }; /* no info leak */
		if ((len -= sizeof(gso)) < 0)
			return -EINVAL;

		if (skb_is_gso(skb)) {
			struct skb_shared_info *sinfo = skb_shinfo(skb);

			/* This is a hint as to how much should be linear. */
			gso.hdr_len = skb_headlen(skb);
			gso.gso_size = sinfo->gso_size;
			if (sinfo->gso_type & SKB_GSO_TCPV4)
				gso.gso_type = VIRTIO_NET_HDR_GSO_TCPV4;
			else if (sinfo->gso_type & SKB_GSO_TCPV6)
				gso.gso_type = VIRTIO_NET_HDR_GSO_TCPV6;
			else
				BUG();
			if (sinfo->gso_type & SKB_GSO_TCP_ECN)
				gso.gso_type |= VIRTIO_NET_HDR_GSO_ECN;
		} else
			gso.gso_type = VIRTIO_NET_HDR_GSO_NONE;

		if (skb->ip_summed == CHECKSUM_PARTIAL) {
			gso.flags = VIRTIO_NET_HDR_F_NEEDS_CSUM;
			gso.csum_start = skb->csum_start - skb_headroom(skb);
			gso.csum_offset = skb->csum_offset;
		} /* else everything is zero */

		if (unlikely(memcpy_toiovecend(iv, (void *)&gso, total,
					       sizeof(gso))))
			return -EFAULT;
		total += sizeof(gso);
	}

	len = min_t(int, skb->len, len);

	skb_copy_datagram_const_iovec(skb, 0, iv, total, len);
	total += len;

	tun->dev->stats.tx_packets++;
	tun->dev->stats.tx_bytes += len;

	return total;
}