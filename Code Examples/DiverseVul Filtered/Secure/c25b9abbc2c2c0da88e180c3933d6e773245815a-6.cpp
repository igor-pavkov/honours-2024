void mac_drv_tx_complete(struct s_smc *smc, volatile struct s_smt_fp_txd *txd)
{
	struct sk_buff *skb;

	PRINTK(KERN_INFO "entering mac_drv_tx_complete\n");
	// Check if this TxD points to a skb

	if (!(skb = txd->txd_os.skb)) {
		PRINTK("TXD with no skb assigned.\n");
		return;
	}
	txd->txd_os.skb = NULL;

	// release the DMA mapping
	pci_unmap_single(&smc->os.pdev, txd->txd_os.dma_addr,
			 skb->len, PCI_DMA_TODEVICE);
	txd->txd_os.dma_addr = 0;

	smc->os.MacStat.gen.tx_packets++;	// Count transmitted packets.
	smc->os.MacStat.gen.tx_bytes+=skb->len;	// Count bytes

	// free the skb
	dev_kfree_skb_irq(skb);

	PRINTK(KERN_INFO "leaving mac_drv_tx_complete\n");
}				// mac_drv_tx_complete