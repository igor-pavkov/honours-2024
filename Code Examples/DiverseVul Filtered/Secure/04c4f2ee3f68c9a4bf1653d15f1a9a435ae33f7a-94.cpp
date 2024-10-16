static void vmx_cleanup_l1d_flush(void)
{
	if (vmx_l1d_flush_pages) {
		free_pages((unsigned long)vmx_l1d_flush_pages, L1D_CACHE_ORDER);
		vmx_l1d_flush_pages = NULL;
	}
	/* Restore state so sysfs ignores VMX */
	l1tf_vmx_mitigation = VMENTER_L1D_FLUSH_AUTO;
}