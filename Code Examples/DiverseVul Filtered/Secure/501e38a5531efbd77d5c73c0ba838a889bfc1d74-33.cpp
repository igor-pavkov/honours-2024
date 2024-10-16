static inline void ep0_readable (struct dev_data *dev)
{
	wake_up (&dev->wait);
	kill_fasync (&dev->fasync, SIGIO, POLL_IN);
}