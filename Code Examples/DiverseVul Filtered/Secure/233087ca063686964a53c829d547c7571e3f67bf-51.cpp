static int compat_getfdcstat(int drive,
			    struct compat_floppy_fdc_state __user *arg)
{
	struct compat_floppy_fdc_state v32;
	struct floppy_fdc_state v;

	mutex_lock(&floppy_mutex);
	v = fdc_state[FDC(drive)];
	mutex_unlock(&floppy_mutex);

	memset(&v32, 0, sizeof(struct compat_floppy_fdc_state));
	v32.spec1 = v.spec1;
	v32.spec2 = v.spec2;
	v32.dtr = v.dtr;
	v32.version = v.version;
	v32.dor = v.dor;
	v32.address = v.address;
	v32.rawcmd = v.rawcmd;
	v32.reset = v.reset;
	v32.need_configure = v.need_configure;
	v32.perp_mode = v.perp_mode;
	v32.has_fifo = v.has_fifo;
	v32.driver_version = v.driver_version;
	memcpy(v32.track, v.track, 4);
	if (copy_to_user(arg, &v32, sizeof(struct compat_floppy_fdc_state)))
		return -EFAULT;
	return 0;
}