bool pipe_is_unprivileged_user(void)
{
	return !capable(CAP_SYS_RESOURCE) && !capable(CAP_SYS_ADMIN);
}