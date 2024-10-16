static int need_more_output(int fdc)
{
	int status = wait_til_ready(fdc);

	if (status < 0)
		return -1;

	if (is_ready_state(status))
		return MORE_OUTPUT;

	return result(fdc);
}