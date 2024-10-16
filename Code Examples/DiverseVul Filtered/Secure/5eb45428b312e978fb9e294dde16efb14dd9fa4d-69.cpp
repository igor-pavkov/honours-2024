int lxc_pclose(struct lxc_popen_FILE *fp)
{
	pid_t wait_pid;
	int wstatus = 0;

	if (!fp)
		return -1;

	do {
		wait_pid = waitpid(fp->child_pid, &wstatus, 0);
	} while (wait_pid < 0 && errno == EINTR);

	fclose(fp->f);
	free(fp);

	if (wait_pid < 0)
		return -1;

	return wstatus;
}