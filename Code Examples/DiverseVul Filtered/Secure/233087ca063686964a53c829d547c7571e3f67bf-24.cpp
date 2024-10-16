static int compat_getdrvprm(int drive,
			    struct compat_floppy_drive_params __user *arg)
{
	struct compat_floppy_drive_params v;

	memset(&v, 0, sizeof(struct compat_floppy_drive_params));
	mutex_lock(&floppy_mutex);
	v.cmos = drive_params[drive].cmos;
	v.max_dtr = drive_params[drive].max_dtr;
	v.hlt = drive_params[drive].hlt;
	v.hut = drive_params[drive].hut;
	v.srt = drive_params[drive].srt;
	v.spinup = drive_params[drive].spinup;
	v.spindown = drive_params[drive].spindown;
	v.spindown_offset = drive_params[drive].spindown_offset;
	v.select_delay = drive_params[drive].select_delay;
	v.rps = drive_params[drive].rps;
	v.tracks = drive_params[drive].tracks;
	v.timeout = drive_params[drive].timeout;
	v.interleave_sect = drive_params[drive].interleave_sect;
	v.max_errors = drive_params[drive].max_errors;
	v.flags = drive_params[drive].flags;
	v.read_track = drive_params[drive].read_track;
	memcpy(v.autodetect, drive_params[drive].autodetect,
	       sizeof(v.autodetect));
	v.checkfreq = drive_params[drive].checkfreq;
	v.native_format = drive_params[drive].native_format;
	mutex_unlock(&floppy_mutex);

	if (copy_to_user(arg, &v, sizeof(struct compat_floppy_drive_params)))
		return -EFAULT;
	return 0;
}