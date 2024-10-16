static int activate_ep_files (struct dev_data *dev)
{
	struct usb_ep	*ep;
	struct ep_data	*data;

	gadget_for_each_ep (ep, dev->gadget) {

		data = kzalloc(sizeof(*data), GFP_KERNEL);
		if (!data)
			goto enomem0;
		data->state = STATE_EP_DISABLED;
		mutex_init(&data->lock);
		init_waitqueue_head (&data->wait);

		strncpy (data->name, ep->name, sizeof (data->name) - 1);
		refcount_set (&data->count, 1);
		data->dev = dev;
		get_dev (dev);

		data->ep = ep;
		ep->driver_data = data;

		data->req = usb_ep_alloc_request (ep, GFP_KERNEL);
		if (!data->req)
			goto enomem1;

		data->dentry = gadgetfs_create_file (dev->sb, data->name,
				data, &ep_io_operations);
		if (!data->dentry)
			goto enomem2;
		list_add_tail (&data->epfiles, &dev->epfiles);
	}
	return 0;

enomem2:
	usb_ep_free_request (ep, data->req);
enomem1:
	put_dev (dev);
	kfree (data);
enomem0:
	DBG (dev, "%s enomem\n", __func__);
	destroy_ep_files (dev);
	return -ENOMEM;
}