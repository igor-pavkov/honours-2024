static int compat_getdrvprm(int drive,
			    struct compat_floppy_drive_params __user *arg)
{
	struct compat_floppy_drive_params v;

	memset(&v, 0, sizeof(struct compat_floppy_drive_params));
	mutex_lock(&floppy_mutex);
	v.cmos = UDP->cmos;
	v.max_dtr = UDP->max_dtr;
	v.hlt = UDP->hlt;
	v.hut = UDP->hut;
	v.srt = UDP->srt;
	v.spinup = UDP->spinup;
	v.spindown = UDP->spindown;
	v.spindown_offset = UDP->spindown_offset;
	v.select_delay = UDP->select_delay;
	v.rps = UDP->rps;
	v.tracks = UDP->tracks;
	v.timeout = UDP->timeout;
	v.interleave_sect = UDP->interleave_sect;
	v.max_errors = UDP->max_errors;
	v.flags = UDP->flags;
	v.read_track = UDP->read_track;
	memcpy(v.autodetect, UDP->autodetect, sizeof(v.autodetect));
	v.checkfreq = UDP->checkfreq;
	v.native_format = UDP->native_format;
	mutex_unlock(&floppy_mutex);

	if (copy_to_user(arg, &v, sizeof(struct compat_floppy_drive_params)))
		return -EFAULT;
	return 0;
}