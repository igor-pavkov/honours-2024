void kvm_inject_page_fault(struct kvm_vcpu *vcpu, struct x86_exception *fault)
{
	++vcpu->stat.pf_guest;
	vcpu->arch.exception.nested_apf =
		is_guest_mode(vcpu) && fault->async_page_fault;
	if (vcpu->arch.exception.nested_apf) {
		vcpu->arch.apf.nested_apf_token = fault->address;
		kvm_queue_exception_e(vcpu, PF_VECTOR, fault->error_code);
	} else {
		kvm_queue_exception_e_p(vcpu, PF_VECTOR, fault->error_code,
					fault->address);
	}
}