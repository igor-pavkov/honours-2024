static int do_lock_ioctl(struct comedi_device *dev, unsigned int arg,
			 void *file)
{
	int ret = 0;
	unsigned long flags;
	struct comedi_subdevice *s;

	if (arg >= dev->n_subdevices)
		return -EINVAL;
	s = dev->subdevices + arg;

	spin_lock_irqsave(&s->spin_lock, flags);
	if (s->busy || s->lock)
		ret = -EBUSY;
	else
		s->lock = file;
	spin_unlock_irqrestore(&s->spin_lock, flags);

#if 0
	if (ret < 0)
		return ret;

	if (s->lock_f)
		ret = s->lock_f(dev, s);
#endif

	return ret;
}