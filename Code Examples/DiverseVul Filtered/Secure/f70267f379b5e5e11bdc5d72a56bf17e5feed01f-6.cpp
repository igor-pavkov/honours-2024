void sas_free_device(struct kref *kref)
{
	struct domain_device *dev = container_of(kref, typeof(*dev), kref);

	put_device(&dev->rphy->dev);
	dev->rphy = NULL;

	if (dev->parent)
		sas_put_device(dev->parent);

	sas_port_put_phy(dev->phy);
	dev->phy = NULL;

	/* remove the phys and ports, everything else should be gone */
	if (dev_is_expander(dev->dev_type))
		kfree(dev->ex_dev.ex_phy);

	if (dev_is_sata(dev) && dev->sata_dev.ap) {
		ata_sas_tport_delete(dev->sata_dev.ap);
		ata_sas_port_destroy(dev->sata_dev.ap);
		ata_host_put(dev->sata_dev.ata_host);
		dev->sata_dev.ata_host = NULL;
		dev->sata_dev.ap = NULL;
	}

	kfree(dev);
}