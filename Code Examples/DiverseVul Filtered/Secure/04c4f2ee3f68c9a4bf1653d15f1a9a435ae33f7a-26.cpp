static int vmx_set_guest_uret_msr(struct vcpu_vmx *vmx,
				  struct vmx_uret_msr *msr, u64 data)
{
	int ret = 0;

	u64 old_msr_data = msr->data;
	msr->data = data;
	if (msr - vmx->guest_uret_msrs < vmx->nr_active_uret_msrs) {
		preempt_disable();
		ret = kvm_set_user_return_msr(msr->slot, msr->data, msr->mask);
		preempt_enable();
		if (ret)
			msr->data = old_msr_data;
	}
	return ret;
}