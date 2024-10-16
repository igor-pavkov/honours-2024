int mingw_raise(int sig)
{
	switch (sig) {
	case SIGALRM:
		if (timer_fn == SIG_DFL) {
			if (isatty(STDERR_FILENO))
				fputs("Alarm clock\n", stderr);
			exit(128 + SIGALRM);
		} else if (timer_fn != SIG_IGN)
			timer_fn(SIGALRM);
		return 0;

	case SIGINT:
		if (sigint_fn == SIG_DFL)
			exit(128 + SIGINT);
		else if (sigint_fn != SIG_IGN)
			sigint_fn(SIGINT);
		return 0;

	default:
		return raise(sig);
	}
}