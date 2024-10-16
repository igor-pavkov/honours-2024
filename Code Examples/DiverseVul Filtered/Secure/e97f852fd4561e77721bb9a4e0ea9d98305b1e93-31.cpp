void kvm_arch_async_page_present(struct kvm_vcpu *vcpu,
				 struct kvm_async_pf *work)
{
	struct x86_exception fault;
	u32 val;

	if (work->wakeup_all)
		work->arch.token = ~0; /* broadcast wakeup */
	else
		kvm_del_async_pf_gfn(vcpu, work->arch.gfn);
	trace_kvm_async_pf_ready(work->arch.token, work->gva);

	if (vcpu->arch.apf.msr_val & KVM_ASYNC_PF_ENABLED &&
	    !apf_get_user(vcpu, &val)) {
		if (val == KVM_PV_REASON_PAGE_NOT_PRESENT &&
		    vcpu->arch.exception.pending &&
		    vcpu->arch.exception.nr == PF_VECTOR &&
		    !apf_put_user(vcpu, 0)) {
			vcpu->arch.exception.injected = false;
			vcpu->arch.exception.pending = false;
			vcpu->arch.exception.nr = 0;
			vcpu->arch.exception.has_error_code = false;
			vcpu->arch.exception.error_code = 0;
			vcpu->arch.exception.has_payload = false;
			vcpu->arch.exception.payload = 0;
		} else if (!apf_put_user(vcpu, KVM_PV_REASON_PAGE_READY)) {
			fault.vector = PF_VECTOR;
			fault.error_code_valid = true;
			fault.error_code = 0;
			fault.nested_page_fault = false;
			fault.address = work->arch.token;
			fault.async_page_fault = true;
			kvm_inject_page_fault(vcpu, &fault);
		}
	}
	vcpu->arch.apf.halted = false;
	vcpu->arch.mp_state = KVM_MP_STATE_RUNNABLE;
}