void nested_vmx_vcpu_setup(void)
{
	if (enable_shadow_vmcs) {
		/*
		 * At vCPU creation, "VMWRITE to any supported field
		 * in the VMCS" is supported, so use the more
		 * permissive vmx_vmread_bitmap to specify both read
		 * and write permissions for the shadow VMCS.
		 */
		vmcs_write64(VMREAD_BITMAP, __pa(vmx_vmread_bitmap));
		vmcs_write64(VMWRITE_BITMAP, __pa(vmx_vmread_bitmap));
	}
}