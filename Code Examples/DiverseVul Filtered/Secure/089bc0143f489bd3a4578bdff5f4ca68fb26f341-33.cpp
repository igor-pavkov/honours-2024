static void make_response(struct xen_blkif_ring *ring, u64 id,
			  unsigned short op, int st)
{
	struct blkif_response *resp;
	unsigned long     flags;
	union blkif_back_rings *blk_rings;
	int notify;

	spin_lock_irqsave(&ring->blk_ring_lock, flags);
	blk_rings = &ring->blk_rings;
	/* Place on the response ring for the relevant domain. */
	switch (ring->blkif->blk_protocol) {
	case BLKIF_PROTOCOL_NATIVE:
		resp = RING_GET_RESPONSE(&blk_rings->native,
					 blk_rings->native.rsp_prod_pvt);
		break;
	case BLKIF_PROTOCOL_X86_32:
		resp = RING_GET_RESPONSE(&blk_rings->x86_32,
					 blk_rings->x86_32.rsp_prod_pvt);
		break;
	case BLKIF_PROTOCOL_X86_64:
		resp = RING_GET_RESPONSE(&blk_rings->x86_64,
					 blk_rings->x86_64.rsp_prod_pvt);
		break;
	default:
		BUG();
	}

	resp->id        = id;
	resp->operation = op;
	resp->status    = st;

	blk_rings->common.rsp_prod_pvt++;
	RING_PUSH_RESPONSES_AND_CHECK_NOTIFY(&blk_rings->common, notify);
	spin_unlock_irqrestore(&ring->blk_ring_lock, flags);
	if (notify)
		notify_remote_via_irq(ring->irq);
}