static unsigned __stdcall ticktack(void *dummy)
{
	while (WaitForSingleObject(timer_event, timer_interval) == WAIT_TIMEOUT) {
		mingw_raise(SIGALRM);
		if (one_shot)
			break;
	}
	return 0;
}