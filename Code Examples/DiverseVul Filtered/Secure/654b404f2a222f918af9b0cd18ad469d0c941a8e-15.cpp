static void stop_read(struct edgeport_port *edge_port)
{
	unsigned long flags;

	spin_lock_irqsave(&edge_port->ep_lock, flags);

	if (edge_port->ep_read_urb_state == EDGE_READ_URB_RUNNING)
		edge_port->ep_read_urb_state = EDGE_READ_URB_STOPPING;
	edge_port->shadow_mcr &= ~MCR_RTS;

	spin_unlock_irqrestore(&edge_port->ep_lock, flags);
}