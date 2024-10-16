static _INLINE_ void check_modem_status(struct mp_port *mtpt)
{
	int status;

	status = serial_in(mtpt, UART_MSR);

	if ((status & UART_MSR_ANY_DELTA) == 0)
		return;

	if (status & UART_MSR_TERI)
		mtpt->port.icount.rng++;
	if (status & UART_MSR_DDSR)
		mtpt->port.icount.dsr++;
	if (status & UART_MSR_DDCD)
		sb_uart_handle_dcd_change(&mtpt->port, status & UART_MSR_DCD);
	if (status & UART_MSR_DCTS)
		sb_uart_handle_cts_change(&mtpt->port, status & UART_MSR_CTS);

	wake_up_interruptible(&mtpt->port.info->delta_msr_wait);
}