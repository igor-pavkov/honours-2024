static int ati_remote2_set_mode_mask(const char *val,
				     const struct kernel_param *kp)
{
	pr_debug("%s()\n", __func__);

	return ati_remote2_set_mask(val, kp, ATI_REMOTE2_MAX_MODE_MASK);
}