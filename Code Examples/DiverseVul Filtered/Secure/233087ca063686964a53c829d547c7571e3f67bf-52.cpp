static void set_floppy(int drive)
{
	int type = ITYPE(drive_state[drive].fd_device);

	if (type)
		_floppy = floppy_type + type;
	else
		_floppy = current_type[drive];
}