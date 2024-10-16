static irqreturn_t b43_interrupt_thread_handler(int irq, void *dev_id)
{
	struct b43_wldev *dev = dev_id;

	mutex_lock(&dev->wl->mutex);
	b43_do_interrupt_thread(dev);
	mmiowb();
	mutex_unlock(&dev->wl->mutex);

	return IRQ_HANDLED;
}