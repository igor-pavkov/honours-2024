int comedi_alloc_board_minor(struct device *hardware_device)
{
	unsigned long flags;
	struct comedi_device_file_info *info;
	struct device *csdev;
	unsigned i;
	int retval;

	info = kzalloc(sizeof(struct comedi_device_file_info), GFP_KERNEL);
	if (info == NULL)
		return -ENOMEM;
	info->device = kzalloc(sizeof(struct comedi_device), GFP_KERNEL);
	if (info->device == NULL) {
		kfree(info);
		return -ENOMEM;
	}
	comedi_device_init(info->device);
	spin_lock_irqsave(&comedi_file_info_table_lock, flags);
	for (i = 0; i < COMEDI_NUM_BOARD_MINORS; ++i) {
		if (comedi_file_info_table[i] == NULL) {
			comedi_file_info_table[i] = info;
			break;
		}
	}
	spin_unlock_irqrestore(&comedi_file_info_table_lock, flags);
	if (i == COMEDI_NUM_BOARD_MINORS) {
		comedi_device_cleanup(info->device);
		kfree(info->device);
		kfree(info);
		printk(KERN_ERR
		       "comedi: error: "
		       "ran out of minor numbers for board device files.\n");
		return -EBUSY;
	}
	info->device->minor = i;
	csdev = COMEDI_DEVICE_CREATE(comedi_class, NULL,
				     MKDEV(COMEDI_MAJOR, i), NULL,
				     hardware_device, "comedi%i", i);
	if (!IS_ERR(csdev))
		info->device->class_dev = csdev;
	dev_set_drvdata(csdev, info);
	retval = device_create_file(csdev, &dev_attr_max_read_buffer_kb);
	if (retval) {
		printk(KERN_ERR
		       "comedi: "
		       "failed to create sysfs attribute file \"%s\".\n",
		       dev_attr_max_read_buffer_kb.attr.name);
		comedi_free_board_minor(i);
		return retval;
	}
	retval = device_create_file(csdev, &dev_attr_read_buffer_kb);
	if (retval) {
		printk(KERN_ERR
		       "comedi: "
		       "failed to create sysfs attribute file \"%s\".\n",
		       dev_attr_read_buffer_kb.attr.name);
		comedi_free_board_minor(i);
		return retval;
	}
	retval = device_create_file(csdev, &dev_attr_max_write_buffer_kb);
	if (retval) {
		printk(KERN_ERR
		       "comedi: "
		       "failed to create sysfs attribute file \"%s\".\n",
		       dev_attr_max_write_buffer_kb.attr.name);
		comedi_free_board_minor(i);
		return retval;
	}
	retval = device_create_file(csdev, &dev_attr_write_buffer_kb);
	if (retval) {
		printk(KERN_ERR
		       "comedi: "
		       "failed to create sysfs attribute file \"%s\".\n",
		       dev_attr_write_buffer_kb.attr.name);
		comedi_free_board_minor(i);
		return retval;
	}
	return i;
}