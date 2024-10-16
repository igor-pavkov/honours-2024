static int rfcomm_tty_write_room(struct tty_struct *tty)
{
	struct rfcomm_dev *dev = (struct rfcomm_dev *) tty->driver_data;
	int room;

	BT_DBG("tty %p", tty);

	if (!dev || !dev->dlc)
		return 0;

	room = rfcomm_room(dev->dlc) - atomic_read(&dev->wmem_alloc);
	if (room < 0)
		room = 0;

	return room;
}