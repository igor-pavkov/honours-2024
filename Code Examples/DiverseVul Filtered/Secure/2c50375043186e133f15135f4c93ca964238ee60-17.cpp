void sigusr1_handler(int sig)
{

	/* Linux has "one-shot" signals, reinstall the signal handler */
	signal(SIGUSR1, sigusr1_handler);

	dlog(LOG_DEBUG, 4, "sigusr1_handler called");

	sigusr1_received = 1;

}