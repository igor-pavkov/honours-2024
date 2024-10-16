static int compat_setdrvprm(int drive,
			    struct compat_floppy_drive_params __user *arg)
{
	struct compat_floppy_drive_params v;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;
	if (copy_from_user(&v, arg, sizeof(struct compat_floppy_drive_params)))
		return -EFAULT;
	if (!valid_floppy_drive_params(v.autodetect, v.native_format))
		return -EINVAL;
	mutex_lock(&floppy_mutex);
	drive_params[drive].cmos = v.cmos;
	drive_params[drive].max_dtr = v.max_dtr;
	drive_params[drive].hlt = v.hlt;
	drive_params[drive].hut = v.hut;
	drive_params[drive].srt = v.srt;
	drive_params[drive].spinup = v.spinup;
	drive_params[drive].spindown = v.spindown;
	drive_params[drive].spindown_offset = v.spindown_offset;
	drive_params[drive].select_delay = v.select_delay;
	drive_params[drive].rps = v.rps;
	drive_params[drive].tracks = v.tracks;
	drive_params[drive].timeout = v.timeout;
	drive_params[drive].interleave_sect = v.interleave_sect;
	drive_params[drive].max_errors = v.max_errors;
	drive_params[drive].flags = v.flags;
	drive_params[drive].read_track = v.read_track;
	memcpy(drive_params[drive].autodetect, v.autodetect,
	       sizeof(v.autodetect));
	drive_params[drive].checkfreq = v.checkfreq;
	drive_params[drive].native_format = v.native_format;
	mutex_unlock(&floppy_mutex);
	return 0;
}