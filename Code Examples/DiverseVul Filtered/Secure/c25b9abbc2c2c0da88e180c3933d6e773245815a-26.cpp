void dma_complete(struct s_smc *smc, volatile union s_fp_descr *descr, int flag)
{
	/* For TX buffers, there are two cases.  If it is an SMT transmit
	 * buffer, there is nothing to do since we use consistent memory
	 * for the 'shared' memory area.  The other case is for normal
	 * transmit packets given to us by the networking stack, and in
	 * that case we cleanup the PCI DMA mapping in mac_drv_tx_complete
	 * below.
	 *
	 * For RX buffers, we have to unmap dynamic PCI DMA mappings here
	 * because the hardware module is about to potentially look at
	 * the contents of the buffer.  If we did not call the PCI DMA
	 * unmap first, the hardware module could read inconsistent data.
	 */
	if (flag & DMA_WR) {
		skfddi_priv *bp = &smc->os;
		volatile struct s_smt_fp_rxd *r = &descr->r;

		/* If SKB is NULL, we used the local buffer. */
		if (r->rxd_os.skb && r->rxd_os.dma_addr) {
			int MaxFrameSize = bp->MaxFrameSize;

			pci_unmap_single(&bp->pdev, r->rxd_os.dma_addr,
					 MaxFrameSize, PCI_DMA_FROMDEVICE);
			r->rxd_os.dma_addr = 0;
		}
	}
}				// dma_complete