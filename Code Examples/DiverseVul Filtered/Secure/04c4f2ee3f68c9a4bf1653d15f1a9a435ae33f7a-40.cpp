static int vmentry_l1d_flush_set(const char *s, const struct kernel_param *kp)
{
	int l1tf, ret;

	l1tf = vmentry_l1d_flush_parse(s);
	if (l1tf < 0)
		return l1tf;

	if (!boot_cpu_has(X86_BUG_L1TF))
		return 0;

	/*
	 * Has vmx_init() run already? If not then this is the pre init
	 * parameter parsing. In that case just store the value and let
	 * vmx_init() do the proper setup after enable_ept has been
	 * established.
	 */
	if (l1tf_vmx_mitigation == VMENTER_L1D_FLUSH_AUTO) {
		vmentry_l1d_flush_param = l1tf;
		return 0;
	}

	mutex_lock(&vmx_l1d_flush_mutex);
	ret = vmx_setup_l1d_flush(l1tf);
	mutex_unlock(&vmx_l1d_flush_mutex);
	return ret;
}