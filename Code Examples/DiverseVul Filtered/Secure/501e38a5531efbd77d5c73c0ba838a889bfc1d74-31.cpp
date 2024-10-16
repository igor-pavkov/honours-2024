static void destroy_ep_files (struct dev_data *dev)
{
	DBG (dev, "%s %d\n", __func__, dev->state);

	/* dev->state must prevent interference */
	spin_lock_irq (&dev->lock);
	while (!list_empty(&dev->epfiles)) {
		struct ep_data	*ep;
		struct inode	*parent;
		struct dentry	*dentry;

		/* break link to FS */
		ep = list_first_entry (&dev->epfiles, struct ep_data, epfiles);
		list_del_init (&ep->epfiles);
		spin_unlock_irq (&dev->lock);

		dentry = ep->dentry;
		ep->dentry = NULL;
		parent = d_inode(dentry->d_parent);

		/* break link to controller */
		mutex_lock(&ep->lock);
		if (ep->state == STATE_EP_ENABLED)
			(void) usb_ep_disable (ep->ep);
		ep->state = STATE_EP_UNBOUND;
		usb_ep_free_request (ep->ep, ep->req);
		ep->ep = NULL;
		mutex_unlock(&ep->lock);

		wake_up (&ep->wait);
		put_ep (ep);

		/* break link to dcache */
		inode_lock(parent);
		d_delete (dentry);
		dput (dentry);
		inode_unlock(parent);

		spin_lock_irq (&dev->lock);
	}
	spin_unlock_irq (&dev->lock);
}