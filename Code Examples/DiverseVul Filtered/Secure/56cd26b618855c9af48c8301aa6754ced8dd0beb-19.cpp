static int serial_ir_tx_carrier(struct rc_dev *dev, u32 carrier)
{
	if (carrier > 500000 || carrier < 20000)
		return -EINVAL;

	serial_ir.carrier = carrier;
	return 0;
}