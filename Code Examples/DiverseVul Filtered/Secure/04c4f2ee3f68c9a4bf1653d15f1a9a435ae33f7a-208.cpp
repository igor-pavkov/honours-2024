void noinstr vmx_update_host_rsp(struct vcpu_vmx *vmx, unsigned long host_rsp)
{
	if (unlikely(host_rsp != vmx->loaded_vmcs->host_state.rsp)) {
		vmx->loaded_vmcs->host_state.rsp = host_rsp;
		vmcs_writel(HOST_RSP, host_rsp);
	}
}