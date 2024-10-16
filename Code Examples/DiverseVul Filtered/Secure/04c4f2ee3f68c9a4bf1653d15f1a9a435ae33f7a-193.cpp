static inline bool pt_can_write_msr(struct vcpu_vmx *vmx)
{
	return vmx_pt_mode_is_host_guest() &&
	       !(vmx->pt_desc.guest.ctl & RTIT_CTL_TRACEEN);
}