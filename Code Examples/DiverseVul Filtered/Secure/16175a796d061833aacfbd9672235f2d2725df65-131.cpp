static inline void vpid_sync_vcpu_all(struct vcpu_vmx *vmx)
{
	if (vmx->vpid == 0)
		return;

	__invvpid(VMX_VPID_EXTENT_SINGLE_CONTEXT, vmx->vpid, 0);
}