void mac_drv_rx_complete(struct s_smc *smc, volatile struct s_smt_fp_rxd *rxd,
			 int frag_count, int len)
{
	skfddi_priv *bp = &smc->os;
	struct sk_buff *skb;
	unsigned char *virt, *cp;
	unsigned short ri;
	u_int RifLength;

	PRINTK(KERN_INFO "entering mac_drv_rx_complete (len=%d)\n", len);
	if (frag_count != 1) {	// This is not allowed to happen.

		printk("fddi: Multi-fragment receive!\n");
		goto RequeueRxd;	// Re-use the given RXD(s).

	}
	skb = rxd->rxd_os.skb;
	if (!skb) {
		PRINTK(KERN_INFO "No skb in rxd\n");
		smc->os.MacStat.gen.rx_errors++;
		goto RequeueRxd;
	}
	virt = skb->data;

	// The DMA mapping was released in dma_complete above.

	dump_data(skb->data, len);

	/*
	 * FDDI Frame format:
	 * +-------+-------+-------+------------+--------+------------+
	 * | FC[1] | DA[6] | SA[6] | RIF[0..18] | LLC[3] | Data[0..n] |
	 * +-------+-------+-------+------------+--------+------------+
	 *
	 * FC = Frame Control
	 * DA = Destination Address
	 * SA = Source Address
	 * RIF = Routing Information Field
	 * LLC = Logical Link Control
	 */

	// Remove Routing Information Field (RIF), if present.

	if ((virt[1 + 6] & FDDI_RII) == 0)
		RifLength = 0;
	else {
		int n;
// goos: RIF removal has still to be tested
		PRINTK(KERN_INFO "RIF found\n");
		// Get RIF length from Routing Control (RC) field.
		cp = virt + FDDI_MAC_HDR_LEN;	// Point behind MAC header.

		ri = ntohs(*((__be16 *) cp));
		RifLength = ri & FDDI_RCF_LEN_MASK;
		if (len < (int) (FDDI_MAC_HDR_LEN + RifLength)) {
			printk("fddi: Invalid RIF.\n");
			goto RequeueRxd;	// Discard the frame.

		}
		virt[1 + 6] &= ~FDDI_RII;	// Clear RII bit.
		// regions overlap

		virt = cp + RifLength;
		for (n = FDDI_MAC_HDR_LEN; n; n--)
			*--virt = *--cp;
		// adjust sbd->data pointer
		skb_pull(skb, RifLength);
		len -= RifLength;
		RifLength = 0;
	}

	// Count statistics.
	smc->os.MacStat.gen.rx_packets++;	// Count indicated receive
						// packets.
	smc->os.MacStat.gen.rx_bytes+=len;	// Count bytes.

	// virt points to header again
	if (virt[1] & 0x01) {	// Check group (multicast) bit.

		smc->os.MacStat.gen.multicast++;
	}

	// deliver frame to system
	rxd->rxd_os.skb = NULL;
	skb_trim(skb, len);
	skb->protocol = fddi_type_trans(skb, bp->dev);

	netif_rx(skb);

	HWM_RX_CHECK(smc, RX_LOW_WATERMARK);
	return;

      RequeueRxd:
	PRINTK(KERN_INFO "Rx: re-queue RXD.\n");
	mac_drv_requeue_rxd(smc, rxd, frag_count);
	smc->os.MacStat.gen.rx_errors++;	// Count receive packets
						// not indicated.

}				// mac_drv_rx_complete