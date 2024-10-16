static int raw_cmd_ioctl(int cmd, void __user *param)
{
	struct floppy_raw_cmd *my_raw_cmd;
	int drive;
	int ret2;
	int ret;

	if (fdc_state[current_fdc].rawcmd <= 1)
		fdc_state[current_fdc].rawcmd = 1;
	for (drive = 0; drive < N_DRIVE; drive++) {
		if (FDC(drive) != current_fdc)
			continue;
		if (drive == current_drive) {
			if (drive_state[drive].fd_ref > 1) {
				fdc_state[current_fdc].rawcmd = 2;
				break;
			}
		} else if (drive_state[drive].fd_ref) {
			fdc_state[current_fdc].rawcmd = 2;
			break;
		}
	}

	if (fdc_state[current_fdc].reset)
		return -EIO;

	ret = raw_cmd_copyin(cmd, param, &my_raw_cmd);
	if (ret) {
		raw_cmd_free(&my_raw_cmd);
		return ret;
	}

	raw_cmd = my_raw_cmd;
	cont = &raw_cmd_cont;
	ret = wait_til_done(floppy_start, true);
	debug_dcl(drive_params[current_drive].flags,
		  "calling disk change from raw_cmd ioctl\n");

	if (ret != -EINTR && fdc_state[current_fdc].reset)
		ret = -EIO;

	drive_state[current_drive].track = NO_TRACK;

	ret2 = raw_cmd_copyout(cmd, param, my_raw_cmd);
	if (!ret)
		ret = ret2;
	raw_cmd_free(&my_raw_cmd);
	return ret;
}