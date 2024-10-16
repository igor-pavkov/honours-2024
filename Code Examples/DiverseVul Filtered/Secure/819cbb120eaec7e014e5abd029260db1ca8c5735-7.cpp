static int do_chaninfo_ioctl(struct comedi_device *dev,
			     struct comedi_chaninfo __user *arg)
{
	struct comedi_subdevice *s;
	struct comedi_chaninfo it;

	if (copy_from_user(&it, arg, sizeof(struct comedi_chaninfo)))
		return -EFAULT;

	if (it.subdev >= dev->n_subdevices)
		return -EINVAL;
	s = dev->subdevices + it.subdev;

	if (it.maxdata_list) {
		if (s->maxdata || !s->maxdata_list)
			return -EINVAL;
		if (copy_to_user(it.maxdata_list, s->maxdata_list,
				 s->n_chan * sizeof(unsigned int)))
			return -EFAULT;
	}

	if (it.flaglist) {
		if (!s->flaglist)
			return -EINVAL;
		if (copy_to_user(it.flaglist, s->flaglist,
				 s->n_chan * sizeof(unsigned int)))
			return -EFAULT;
	}

	if (it.rangelist) {
		int i;

		if (!s->range_table_list)
			return -EINVAL;
		for (i = 0; i < s->n_chan; i++) {
			int x;

			x = (dev->minor << 28) | (it.subdev << 24) | (i << 16) |
			    (s->range_table_list[i]->length);
			if (put_user(x, it.rangelist + i))
				return -EFAULT;
		}
#if 0
		if (copy_to_user(it.rangelist, s->range_type_list,
				 s->n_chan * sizeof(unsigned int)))
			return -EFAULT;
#endif
	}

	return 0;
}