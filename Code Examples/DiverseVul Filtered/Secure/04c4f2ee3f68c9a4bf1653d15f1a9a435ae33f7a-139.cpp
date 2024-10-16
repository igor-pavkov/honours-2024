static int vmentry_l1d_flush_get(char *s, const struct kernel_param *kp)
{
	if (WARN_ON_ONCE(l1tf_vmx_mitigation >= ARRAY_SIZE(vmentry_l1d_param)))
		return sprintf(s, "???\n");

	return sprintf(s, "%s\n", vmentry_l1d_param[l1tf_vmx_mitigation].option);
}