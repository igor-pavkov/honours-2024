static void prepare_vmcs02_early_full(struct vcpu_vmx *vmx,
				      struct vmcs12 *vmcs12)
{
	prepare_vmcs02_constant_state(vmx);

	vmcs_write64(VMCS_LINK_POINTER, -1ull);

	if (enable_vpid) {
		if (nested_cpu_has_vpid(vmcs12) && vmx->nested.vpid02)
			vmcs_write16(VIRTUAL_PROCESSOR_ID, vmx->nested.vpid02);
		else
			vmcs_write16(VIRTUAL_PROCESSOR_ID, vmx->vpid);
	}
}