static void b43_sdio_interrupt_handler(struct b43_wldev *dev)
{
	struct b43_wl *wl = dev->wl;
	irqreturn_t ret;

	mutex_lock(&wl->mutex);

	ret = b43_do_interrupt(dev);
	if (ret == IRQ_WAKE_THREAD)
		b43_do_interrupt_thread(dev);

	mutex_unlock(&wl->mutex);
}