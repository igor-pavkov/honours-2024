static void mp_change_pm(struct sb_uart_state *state, int pm_state)
{
	struct sb_uart_port *port = state->port;
	if (port->ops->pm)
		port->ops->pm(port, pm_state, state->pm_state);
	state->pm_state = pm_state;
}