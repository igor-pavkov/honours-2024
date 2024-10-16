static int digi_write_inb_command(struct usb_serial_port *port,
	unsigned char *buf, int count, unsigned long timeout)
{
	int ret = 0;
	int len;
	struct digi_port *priv = usb_get_serial_port_data(port);
	unsigned char *data = port->write_urb->transfer_buffer;
	unsigned long flags = 0;

	dev_dbg(&port->dev, "digi_write_inb_command: TOP: port=%d, count=%d\n",
		priv->dp_port_num, count);

	if (timeout)
		timeout += jiffies;
	else
		timeout = ULONG_MAX;

	spin_lock_irqsave(&priv->dp_port_lock, flags);
	while (count > 0 && ret == 0) {
		while (priv->dp_write_urb_in_use &&
		       time_before(jiffies, timeout)) {
			cond_wait_interruptible_timeout_irqrestore(
				&port->write_wait, DIGI_RETRY_TIMEOUT,
				&priv->dp_port_lock, flags);
			if (signal_pending(current))
				return -EINTR;
			spin_lock_irqsave(&priv->dp_port_lock, flags);
		}

		/* len must be a multiple of 4 and small enough to */
		/* guarantee the write will send buffered data first, */
		/* so commands are in order with data and not split */
		len = min(count, port->bulk_out_size-2-priv->dp_out_buf_len);
		if (len > 4)
			len &= ~3;

		/* write any buffered data first */
		if (priv->dp_out_buf_len > 0) {
			data[0] = DIGI_CMD_SEND_DATA;
			data[1] = priv->dp_out_buf_len;
			memcpy(data + 2, priv->dp_out_buf,
				priv->dp_out_buf_len);
			memcpy(data + 2 + priv->dp_out_buf_len, buf, len);
			port->write_urb->transfer_buffer_length
				= priv->dp_out_buf_len + 2 + len;
		} else {
			memcpy(data, buf, len);
			port->write_urb->transfer_buffer_length = len;
		}

		ret = usb_submit_urb(port->write_urb, GFP_ATOMIC);
		if (ret == 0) {
			priv->dp_write_urb_in_use = 1;
			priv->dp_out_buf_len = 0;
			count -= len;
			buf += len;
		}

	}
	spin_unlock_irqrestore(&priv->dp_port_lock, flags);

	if (ret)
		dev_err(&port->dev,
			"%s: usb_submit_urb failed, ret=%d, port=%d\n",
			__func__, ret, priv->dp_port_num);
	return ret;
}