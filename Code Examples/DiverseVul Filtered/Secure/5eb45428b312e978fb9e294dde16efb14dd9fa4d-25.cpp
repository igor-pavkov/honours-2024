int set_stdfds(int fd)
{
	int ret;

	if (fd < 0)
		return -1;

	ret = dup2(fd, STDIN_FILENO);
	if (ret < 0)
		return -1;

	ret = dup2(fd, STDOUT_FILENO);
	if (ret < 0)
		return -1;

	ret = dup2(fd, STDERR_FILENO);
	if (ret < 0)
		return -1;

	return 0;
}