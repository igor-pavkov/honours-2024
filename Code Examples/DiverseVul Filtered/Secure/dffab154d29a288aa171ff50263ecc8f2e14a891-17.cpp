static void log_btmp(struct passwd const *pw)
{
	struct utmpx ut;
	struct timeval tv;
	const char *tty_name, *tty_num;

	memset(&ut, 0, sizeof(ut));

	strncpy(ut.ut_user,
		pw && pw->pw_name ? pw->pw_name : "(unknown)",
		sizeof(ut.ut_user));

	get_terminal_name(NULL, &tty_name, &tty_num);
	if (tty_num)
		xstrncpy(ut.ut_id, tty_num, sizeof(ut.ut_id));
	if (tty_name)
		xstrncpy(ut.ut_line, tty_name, sizeof(ut.ut_line));

	gettimeofday(&tv, NULL);
	ut.ut_tv.tv_sec = tv.tv_sec;
	ut.ut_tv.tv_usec = tv.tv_usec;
	ut.ut_type = LOGIN_PROCESS;	/* XXX doesn't matter */
	ut.ut_pid = getpid();

	updwtmpx(_PATH_BTMP, &ut);
}