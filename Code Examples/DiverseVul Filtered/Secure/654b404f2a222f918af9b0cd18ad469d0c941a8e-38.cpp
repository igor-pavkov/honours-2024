static void edge_send(struct usb_serial_port *port, struct tty_struct *tty)
{
	int count, result;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	unsigned long flags;

	spin_lock_irqsave(&edge_port->ep_lock, flags);

	if (edge_port->ep_write_urb_in_use) {
		spin_unlock_irqrestore(&edge_port->ep_lock, flags);
		return;
	}

	count = kfifo_out(&port->write_fifo,
				port->write_urb->transfer_buffer,
				port->bulk_out_size);

	if (count == 0) {
		spin_unlock_irqrestore(&edge_port->ep_lock, flags);
		return;
	}

	edge_port->ep_write_urb_in_use = 1;

	spin_unlock_irqrestore(&edge_port->ep_lock, flags);

	usb_serial_debug_data(&port->dev, __func__, count, port->write_urb->transfer_buffer);

	/* set up our urb */
	port->write_urb->transfer_buffer_length = count;

	/* send the data out the bulk port */
	result = usb_submit_urb(port->write_urb, GFP_ATOMIC);
	if (result) {
		dev_err_console(port,
			"%s - failed submitting write urb, error %d\n",
				__func__, result);
		edge_port->ep_write_urb_in_use = 0;
		/* TODO: reschedule edge_send */
	} else
		edge_port->port->icount.tx += count;

	/*
	 * wakeup any process waiting for writes to complete
	 * there is now more room in the buffer for new writes
	 */
	if (tty)
		tty_wakeup(tty);
}