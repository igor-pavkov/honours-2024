static void tower_check_for_read_packet (struct lego_usb_tower *dev)
{
	spin_lock_irq (&dev->read_buffer_lock);
	if (!packet_timeout
	    || time_after(jiffies, dev->read_last_arrival + dev->packet_timeout_jiffies)
	    || dev->read_buffer_length == read_buffer_size) {
		dev->read_packet_length = dev->read_buffer_length;
	}
	dev->interrupt_in_done = 0;
	spin_unlock_irq (&dev->read_buffer_lock);
}