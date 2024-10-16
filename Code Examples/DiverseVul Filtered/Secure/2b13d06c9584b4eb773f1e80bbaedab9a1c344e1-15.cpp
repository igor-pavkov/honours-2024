static inline port_status_t* get_status(port_t *port)
{
	return &port->card->status->port_status[port->node];
}