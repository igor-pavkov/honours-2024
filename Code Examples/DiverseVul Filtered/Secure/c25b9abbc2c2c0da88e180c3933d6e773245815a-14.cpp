void mac_drv_clear_rxd(struct s_smc *smc, volatile struct s_smt_fp_rxd *rxd,
		       int frag_count)
{

	struct sk_buff *skb;

	PRINTK("entering mac_drv_clear_rxd\n");

	if (frag_count != 1)	// This is not allowed to happen.

		printk("fddi: Multi-fragment clear!\n");

	for (; frag_count > 0; frag_count--) {
		skb = rxd->rxd_os.skb;
		if (skb != NULL) {
			skfddi_priv *bp = &smc->os;
			int MaxFrameSize = bp->MaxFrameSize;

			pci_unmap_single(&bp->pdev, rxd->rxd_os.dma_addr,
					 MaxFrameSize, PCI_DMA_FROMDEVICE);

			dev_kfree_skb(skb);
			rxd->rxd_os.skb = NULL;
		}
		rxd = rxd->rxd_next;	// Next RXD.

	}
}				// mac_drv_clear_rxd