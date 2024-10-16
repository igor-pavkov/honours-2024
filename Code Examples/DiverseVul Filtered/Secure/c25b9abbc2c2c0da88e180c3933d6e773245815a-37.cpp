void mac_drv_fill_rxd(struct s_smc *smc)
{
	int MaxFrameSize;
	unsigned char *v_addr;
	unsigned long b_addr;
	struct sk_buff *skb;
	volatile struct s_smt_fp_rxd *rxd;

	PRINTK(KERN_INFO "entering mac_drv_fill_rxd\n");

	// Walk through the list of free receive buffers, passing receive
	// buffers to the HWM as long as RXDs are available.

	MaxFrameSize = smc->os.MaxFrameSize;
	// Check if there is any RXD left.
	while (HWM_GET_RX_FREE(smc) > 0) {
		PRINTK(KERN_INFO ".\n");

		rxd = HWM_GET_CURR_RXD(smc);
		skb = alloc_skb(MaxFrameSize + 3, GFP_ATOMIC);
		if (skb) {
			// we got a skb
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
			// System has run out of buffer memory, but we want to
			// keep the receiver running in hope of better times.
			// Multiple descriptors may point to this local buffer,
			// so data in it must be considered invalid.
			PRINTK("Queueing invalid buffer!\n");
			v_addr = smc->os.LocalRxBuffer;
			b_addr = smc->os.LocalRxBufferDMA;
		}

		rxd->rxd_os.skb = skb;

		// Pass receive buffer to HWM.
		hwm_rx_frag(smc, v_addr, b_addr, MaxFrameSize,
			    FIRST_FRAG | LAST_FRAG);
	}
	PRINTK(KERN_INFO "leaving mac_drv_fill_rxd\n");
}				// mac_drv_fill_rxd