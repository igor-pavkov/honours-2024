static void tell_sector(void)
{
	pr_cont(": track %d, head %d, sector %d, size %d",
		reply_buffer[R_TRACK], reply_buffer[R_HEAD],
		reply_buffer[R_SECTOR],
		reply_buffer[R_SIZECODE]);
}				/* tell_sector */