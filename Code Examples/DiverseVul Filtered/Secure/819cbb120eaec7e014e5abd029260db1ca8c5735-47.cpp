static int do_cmdtest_ioctl(struct comedi_device *dev,
			    struct comedi_cmd __user *arg, void *file)
{
	struct comedi_cmd user_cmd;
	struct comedi_subdevice *s;
	int ret = 0;
	unsigned int *chanlist = NULL;
	unsigned int __user *chanlist_saver = NULL;

	if (copy_from_user(&user_cmd, arg, sizeof(struct comedi_cmd))) {
		DPRINTK("bad cmd address\n");
		return -EFAULT;
	}
	/* save user's chanlist pointer so it can be restored later */
	chanlist_saver = user_cmd.chanlist;

	if (user_cmd.subdev >= dev->n_subdevices) {
		DPRINTK("%d no such subdevice\n", user_cmd.subdev);
		return -ENODEV;
	}

	s = dev->subdevices + user_cmd.subdev;
	if (s->type == COMEDI_SUBD_UNUSED) {
		DPRINTK("%d not valid subdevice\n", user_cmd.subdev);
		return -EIO;
	}

	if (!s->do_cmd || !s->do_cmdtest) {
		DPRINTK("subdevice %i does not support commands\n",
			user_cmd.subdev);
		return -EIO;
	}

	/* make sure channel/gain list isn't too long */
	if (user_cmd.chanlist_len > s->len_chanlist) {
		DPRINTK("channel/gain list too long %d > %d\n",
			user_cmd.chanlist_len, s->len_chanlist);
		ret = -EINVAL;
		goto cleanup;
	}

	/* load channel/gain list */
	if (user_cmd.chanlist) {
		chanlist =
		    kmalloc(user_cmd.chanlist_len * sizeof(int), GFP_KERNEL);
		if (!chanlist) {
			DPRINTK("allocation failed\n");
			ret = -ENOMEM;
			goto cleanup;
		}

		if (copy_from_user(chanlist, user_cmd.chanlist,
				   user_cmd.chanlist_len * sizeof(int))) {
			DPRINTK("fault reading chanlist\n");
			ret = -EFAULT;
			goto cleanup;
		}

		/* make sure each element in channel/gain list is valid */
		ret = comedi_check_chanlist(s, user_cmd.chanlist_len, chanlist);
		if (ret < 0) {
			DPRINTK("bad chanlist\n");
			goto cleanup;
		}

		user_cmd.chanlist = chanlist;
	}

	ret = s->do_cmdtest(dev, s, &user_cmd);

	/* restore chanlist pointer before copying back */
	user_cmd.chanlist = chanlist_saver;

	if (copy_to_user(arg, &user_cmd, sizeof(struct comedi_cmd))) {
		DPRINTK("bad cmd address\n");
		ret = -EFAULT;
		goto cleanup;
	}
cleanup:
	kfree(chanlist);

	return ret;
}