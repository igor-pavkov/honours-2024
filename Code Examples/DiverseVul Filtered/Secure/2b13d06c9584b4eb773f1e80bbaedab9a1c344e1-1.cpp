static void wanxl_pci_remove_one(struct pci_dev *pdev)
{
	card_t *card = pci_get_drvdata(pdev);
	int i;

	for (i = 0; i < card->n_ports; i++) {
		unregister_hdlc_device(card->ports[i].dev);
		free_netdev(card->ports[i].dev);
	}

	/* unregister and free all host resources */
	if (card->irq)
		free_irq(card->irq, card);

	wanxl_reset(card);

	for (i = 0; i < RX_QUEUE_LENGTH; i++)
		if (card->rx_skbs[i]) {
			pci_unmap_single(card->pdev,
					 card->status->rx_descs[i].address,
					 BUFFER_LENGTH, PCI_DMA_FROMDEVICE);
			dev_kfree_skb(card->rx_skbs[i]);
		}

	if (card->plx)
		iounmap(card->plx);

	if (card->status)
		pci_free_consistent(pdev, sizeof(card_status_t),
				    card->status, card->status_address);

	pci_release_regions(pdev);
	pci_disable_device(pdev);
	pci_set_drvdata(pdev, NULL);
	kfree(card);
}