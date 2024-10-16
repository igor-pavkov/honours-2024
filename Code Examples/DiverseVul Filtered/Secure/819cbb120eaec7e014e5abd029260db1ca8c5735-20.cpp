struct comedi_device_file_info *comedi_get_device_file_info(unsigned minor)
{
	unsigned long flags;
	struct comedi_device_file_info *info;

	BUG_ON(minor >= COMEDI_NUM_MINORS);
	spin_lock_irqsave(&comedi_file_info_table_lock, flags);
	info = comedi_file_info_table[minor];
	spin_unlock_irqrestore(&comedi_file_info_table_lock, flags);
	return info;
}