irqreturn_t xen_blkif_be_int(int irq, void *dev_id)
{
	blkif_notify_work(dev_id);
	return IRQ_HANDLED;
}