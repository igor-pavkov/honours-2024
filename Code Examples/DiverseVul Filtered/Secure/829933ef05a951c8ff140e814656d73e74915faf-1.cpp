remove_card(struct pci_dev *dev)
{
	struct pcilynx *lynx = pci_get_drvdata(dev);
	struct client *client;

	mutex_lock(&card_mutex);
	list_del_init(&lynx->link);
	misc_deregister(&lynx->misc);
	mutex_unlock(&card_mutex);

	reg_write(lynx, PCI_INT_ENABLE, 0);
	free_irq(lynx->pci_device->irq, lynx);

	spin_lock_irq(&lynx->client_list_lock);
	list_for_each_entry(client, &lynx->client_list, link)
		wake_up_interruptible(&client->buffer.wait);
	spin_unlock_irq(&lynx->client_list_lock);

	pci_free_consistent(lynx->pci_device, sizeof(struct pcl),
			    lynx->rcv_start_pcl, lynx->rcv_start_pcl_bus);
	pci_free_consistent(lynx->pci_device, sizeof(struct pcl),
			    lynx->rcv_pcl, lynx->rcv_pcl_bus);
	pci_free_consistent(lynx->pci_device, PAGE_SIZE,
			    lynx->rcv_buffer, lynx->rcv_buffer_bus);

	iounmap(lynx->registers);
	pci_disable_device(dev);
	lynx_put(lynx);
}