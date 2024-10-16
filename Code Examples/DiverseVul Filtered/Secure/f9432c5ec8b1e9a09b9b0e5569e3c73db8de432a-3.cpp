static void rfcomm_dev_destruct(struct tty_port *port)
{
	struct rfcomm_dev *dev = container_of(port, struct rfcomm_dev, port);
	struct rfcomm_dlc *dlc = dev->dlc;

	BT_DBG("dev %p dlc %p", dev, dlc);

	/* Refcount should only hit zero when called from rfcomm_dev_del()
	   which will have taken us off the list. Everything else are
	   refcounting bugs. */
	BUG_ON(!list_empty(&dev->list));

	rfcomm_dlc_lock(dlc);
	/* Detach DLC if it's owned by this dev */
	if (dlc->owner == dev)
		dlc->owner = NULL;
	rfcomm_dlc_unlock(dlc);

	rfcomm_dlc_put(dlc);

	tty_unregister_device(rfcomm_tty_driver, dev->id);

	kfree(dev);

	/* It's safe to call module_put() here because socket still
	   holds reference to this module. */
	module_put(THIS_MODULE);
}