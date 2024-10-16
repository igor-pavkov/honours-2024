static int rfcomm_dev_add(struct rfcomm_dev_req *req, struct rfcomm_dlc *dlc)
{
	struct rfcomm_dev *dev, *entry;
	struct list_head *head = &rfcomm_dev_list;
	int err = 0;

	BT_DBG("id %d channel %d", req->dev_id, req->channel);

	dev = kzalloc(sizeof(struct rfcomm_dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	spin_lock(&rfcomm_dev_lock);

	if (req->dev_id < 0) {
		dev->id = 0;

		list_for_each_entry(entry, &rfcomm_dev_list, list) {
			if (entry->id != dev->id)
				break;

			dev->id++;
			head = &entry->list;
		}
	} else {
		dev->id = req->dev_id;

		list_for_each_entry(entry, &rfcomm_dev_list, list) {
			if (entry->id == dev->id) {
				err = -EADDRINUSE;
				goto out;
			}

			if (entry->id > dev->id - 1)
				break;

			head = &entry->list;
		}
	}

	if ((dev->id < 0) || (dev->id > RFCOMM_MAX_DEV - 1)) {
		err = -ENFILE;
		goto out;
	}

	sprintf(dev->name, "rfcomm%d", dev->id);

	list_add(&dev->list, head);

	bacpy(&dev->src, &req->src);
	bacpy(&dev->dst, &req->dst);
	dev->channel = req->channel;

	dev->flags = req->flags &
		((1 << RFCOMM_RELEASE_ONHUP) | (1 << RFCOMM_REUSE_DLC));

	tty_port_init(&dev->port);
	dev->port.ops = &rfcomm_port_ops;
	init_waitqueue_head(&dev->wait);

	skb_queue_head_init(&dev->pending);

	rfcomm_dlc_lock(dlc);

	if (req->flags & (1 << RFCOMM_REUSE_DLC)) {
		struct sock *sk = dlc->owner;
		struct sk_buff *skb;

		BUG_ON(!sk);

		rfcomm_dlc_throttle(dlc);

		while ((skb = skb_dequeue(&sk->sk_receive_queue))) {
			skb_orphan(skb);
			skb_queue_tail(&dev->pending, skb);
			atomic_sub(skb->len, &sk->sk_rmem_alloc);
		}
	}

	dlc->data_ready   = rfcomm_dev_data_ready;
	dlc->state_change = rfcomm_dev_state_change;
	dlc->modem_status = rfcomm_dev_modem_status;

	dlc->owner = dev;
	dev->dlc   = dlc;

	rfcomm_dev_modem_status(dlc, dlc->remote_v24_sig);

	rfcomm_dlc_unlock(dlc);

	/* It's safe to call __module_get() here because socket already
	   holds reference to this module. */
	__module_get(THIS_MODULE);

out:
	spin_unlock(&rfcomm_dev_lock);

	if (err < 0)
		goto free;

	dev->tty_dev = tty_register_device(rfcomm_tty_driver, dev->id, NULL);

	if (IS_ERR(dev->tty_dev)) {
		err = PTR_ERR(dev->tty_dev);
		list_del(&dev->list);
		goto free;
	}

	dev_set_drvdata(dev->tty_dev, dev);

	if (device_create_file(dev->tty_dev, &dev_attr_address) < 0)
		BT_ERR("Failed to create address attribute");

	if (device_create_file(dev->tty_dev, &dev_attr_channel) < 0)
		BT_ERR("Failed to create channel attribute");

	return dev->id;

free:
	kfree(dev);
	return err;
}