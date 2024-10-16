void mac_drv_requeue_rxd(struct s_smc *smc, volatile struct s_smt_fp_rxd *rxd,
			 int frag_count)
{
	volatile struct s_smt_fp_rxd *next_rxd;
	volatile struct s_smt_fp_rxd *src_rxd;
	struct sk_buff *skb;
	int MaxFrameSize;
	unsigned char *v_addr;
	dma_addr_t b_addr;

	if (frag_count != 1)	// This is not allowed to happen.

		printk("fddi: Multi-fragment requeue!\n");

	MaxFrameSize = smc->os.MaxFrameSize;
	src_rxd = rxd;
	for (; frag_count > 0; frag_count--) {
		next_rxd = src_rxd->rxd_next;
		rxd = HWM_GET_CURR_RXD(smc);

		skb = src_rxd->rxd_os.skb;
		if (skb == NULL) {	// this should not happen

			PRINTK("Requeue with no skb in rxd!\n");
			skb = alloc_skb(MaxFrameSize + 3, GFP_ATOMIC);
			if (skb) {
				// we got a skb
				rxd->rxd_os.skb = skb;
				skb_reserve(skb, 3);
				skb_put(skb, MaxFrameSize);
				v_addr = skb->data;
				b_addr = pci_map_single(&smc->os.pdev,
							v_addr,
							MaxFrameSize,
							PCI_DMA_FROMDEVICE);
				rxd->rxd_os.dma_addr = b_addr;
			} else {
				// no skb available, use local buffer
				PRINTK("Queueing invalid buffer!\n");
				rxd->rxd_os.skb = NULL;
				v_addr = smc->os.LocalRxBuffer;
				b_addr = smc->os.LocalRxBufferDMA;
			}
		} else {
			// we use skb from old rxd
			rxd->rxd_os.skb = skb;
			v_addr = skb->data;
			b_addr = pci_map_single(&smc->os.pdev,
						v_addr,
						MaxFrameSize,
						PCI_DMA_FROMDEVICE);
			rxd->rxd_os.dma_addr = b_addr;
		}
		hwm_rx_frag(smc, v_addr, b_addr, MaxFrameSize,
			    FIRST_FRAG | LAST_FRAG);

		src_rxd = next_rxd;
	}
}				// mac_drv_requeue_rxd