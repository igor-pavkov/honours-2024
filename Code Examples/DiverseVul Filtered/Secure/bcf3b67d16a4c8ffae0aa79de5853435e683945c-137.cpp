static int megasas_mgmt_ioctl_aen(struct file *file, unsigned long arg)
{
	struct megasas_instance *instance;
	struct megasas_aen aen;
	int error;

	if (file->private_data != file) {
		printk(KERN_DEBUG "megasas: fasync_helper was not "
		       "called first\n");
		return -EINVAL;
	}

	if (copy_from_user(&aen, (void __user *)arg, sizeof(aen)))
		return -EFAULT;

	instance = megasas_lookup_instance(aen.host_no);

	if (!instance)
		return -ENODEV;

	if (atomic_read(&instance->adprecovery) == MEGASAS_HW_CRITICAL_ERROR) {
		return -ENODEV;
	}

	if (instance->unload == 1) {
		return -ENODEV;
	}

	if  (megasas_wait_for_adapter_operational(instance))
		return -ENODEV;

	mutex_lock(&instance->reset_mutex);
	error = megasas_register_aen(instance, aen.seq_num,
				     aen.class_locale_word);
	mutex_unlock(&instance->reset_mutex);
	return error;
}