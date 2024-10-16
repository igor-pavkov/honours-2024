static void send_queued_packets(struct s_smc *smc)
{
	skfddi_priv *bp = &smc->os;
	struct sk_buff *skb;
	unsigned char fc;
	int queue;
	struct s_smt_fp_txd *txd;	// Current TxD.
	dma_addr_t dma_address;
	unsigned long Flags;

	int frame_status;	// HWM tx frame status.

	PRINTK(KERN_INFO "send queued packets\n");
	for (;;) {
		// send first buffer from queue
		skb = skb_dequeue(&bp->SendSkbQueue);

		if (!skb) {
			PRINTK(KERN_INFO "queue empty\n");
			return;
		}		// queue empty !

		spin_lock_irqsave(&bp->DriverLock, Flags);
		fc = skb->data[0];
		queue = (fc & FC_SYNC_BIT) ? QUEUE_S : QUEUE_A0;
#ifdef ESS
		// Check if the frame may/must be sent as a synchronous frame.

		if ((fc & ~(FC_SYNC_BIT | FC_LLC_PRIOR)) == FC_ASYNC_LLC) {
			// It's an LLC frame.
			if (!smc->ess.sync_bw_available)
				fc &= ~FC_SYNC_BIT; // No bandwidth available.

			else {	// Bandwidth is available.

				if (smc->mib.fddiESSSynchTxMode) {
					// Send as sync. frame.
					fc |= FC_SYNC_BIT;
				}
			}
		}
#endif				// ESS
		frame_status = hwm_tx_init(smc, fc, 1, skb->len, queue);

		if ((frame_status & (LOC_TX | LAN_TX)) == 0) {
			// Unable to send the frame.

			if ((frame_status & RING_DOWN) != 0) {
				// Ring is down.
				PRINTK("Tx attempt while ring down.\n");
			} else if ((frame_status & OUT_OF_TXD) != 0) {
				PRINTK("%s: out of TXDs.\n", bp->dev->name);
			} else {
				PRINTK("%s: out of transmit resources",
					bp->dev->name);
			}

			// Note: We will retry the operation as soon as
			// transmit resources become available.
			skb_queue_head(&bp->SendSkbQueue, skb);
			spin_unlock_irqrestore(&bp->DriverLock, Flags);
			return;	// Packet has been queued.

		}		// if (unable to send frame)

		bp->QueueSkb++;	// one packet less in local queue

		// source address in packet ?
		CheckSourceAddress(skb->data, smc->hw.fddi_canon_addr.a);

		txd = (struct s_smt_fp_txd *) HWM_GET_CURR_TXD(smc, queue);

		dma_address = pci_map_single(&bp->pdev, skb->data,
					     skb->len, PCI_DMA_TODEVICE);
		if (frame_status & LAN_TX) {
			txd->txd_os.skb = skb;			// save skb
			txd->txd_os.dma_addr = dma_address;	// save dma mapping
		}
		hwm_tx_frag(smc, skb->data, dma_address, skb->len,
                      frame_status | FIRST_FRAG | LAST_FRAG | EN_IRQ_EOF);

		if (!(frame_status & LAN_TX)) {		// local only frame
			pci_unmap_single(&bp->pdev, dma_address,
					 skb->len, PCI_DMA_TODEVICE);
			dev_kfree_skb_irq(skb);
		}
		spin_unlock_irqrestore(&bp->DriverLock, Flags);
	}			// for

	return;			// never reached

}				// send_queued_packets