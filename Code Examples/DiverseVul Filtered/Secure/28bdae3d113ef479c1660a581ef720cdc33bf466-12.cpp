static void drop_privs(void)
{
	if (getuid() != 0) {
		oldfsuid = setfsuid(getuid());
		oldfsgid = setfsgid(getgid());
	}
}