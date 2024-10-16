static void prb_open_block(struct tpacket_kbdq_core *pkc1,
	struct tpacket_block_desc *pbd1)
{
	struct timespec64 ts;
	struct tpacket_hdr_v1 *h1 = &pbd1->hdr.bh1;

	smp_rmb();

	/* We could have just memset this but we will lose the
	 * flexibility of making the priv area sticky
	 */

	BLOCK_SNUM(pbd1) = pkc1->knxt_seq_num++;
	BLOCK_NUM_PKTS(pbd1) = 0;
	BLOCK_LEN(pbd1) = BLK_PLUS_PRIV(pkc1->blk_sizeof_priv);

	ktime_get_real_ts64(&ts);

	h1->ts_first_pkt.ts_sec = ts.tv_sec;
	h1->ts_first_pkt.ts_nsec = ts.tv_nsec;

	pkc1->pkblk_start = (char *)pbd1;
	pkc1->nxt_offset = pkc1->pkblk_start + BLK_PLUS_PRIV(pkc1->blk_sizeof_priv);

	BLOCK_O2FP(pbd1) = (__u32)BLK_PLUS_PRIV(pkc1->blk_sizeof_priv);
	BLOCK_O2PRIV(pbd1) = BLK_HDR_LEN;

	pbd1->version = pkc1->version;
	pkc1->prev = pkc1->nxt_offset;
	pkc1->pkblk_end = pkc1->pkblk_start + pkc1->kblk_size;

	prb_thaw_queue(pkc1);
	_prb_refresh_rx_retire_blk_timer(pkc1);

	smp_wmb();
}