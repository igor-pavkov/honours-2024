static void restore_privs(void)
{
	if (getuid() != 0) {
		setfsuid(oldfsuid);
		setfsgid(oldfsgid);
	}
}