void nested_vmx_hardware_unsetup(void)
{
	int i;

	if (enable_shadow_vmcs) {
		for (i = 0; i < VMX_BITMAP_NR; i++)
			free_page((unsigned long)vmx_bitmap[i]);
	}
}