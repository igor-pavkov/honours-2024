static int param_set_scroll(const char *val, const struct kernel_param *kp)
{
	ypan = 0;

	if (!strcmp(val, "redraw"))
		ypan = 0;
	else if (!strcmp(val, "ypan"))
		ypan = 1;
	else if (!strcmp(val, "ywrap"))
		ypan = 2;
	else
		return -EINVAL;

	return 0;
}