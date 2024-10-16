static int restart_read(struct edgeport_port *edge_port)
{
	struct urb *urb;
	int status = 0;
	unsigned long flags;

	spin_lock_irqsave(&edge_port->ep_lock, flags);

	if (edge_port->ep_read_urb_state == EDGE_READ_URB_STOPPED) {
		urb = edge_port->port->read_urb;
		status = usb_submit_urb(urb, GFP_ATOMIC);
	}
	edge_port->ep_read_urb_state = EDGE_READ_URB_RUNNING;
	edge_port->shadow_mcr |= MCR_RTS;

	spin_unlock_irqrestore(&edge_port->ep_lock, flags);

	return status;
}