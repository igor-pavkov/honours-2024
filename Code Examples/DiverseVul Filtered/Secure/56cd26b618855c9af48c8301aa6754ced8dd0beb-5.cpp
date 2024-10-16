static int serial_ir_tx_duty_cycle(struct rc_dev *dev, u32 cycle)
{
	serial_ir.duty_cycle = cycle;
	return 0;
}