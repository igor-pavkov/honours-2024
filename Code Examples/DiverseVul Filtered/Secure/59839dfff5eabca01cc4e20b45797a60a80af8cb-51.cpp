void kvm_inject_page_fault(struct kvm_vcpu *vcpu, unsigned long addr,
			   u32 error_code)
{
	++vcpu->stat.pf_guest;

	if (vcpu->arch.exception.pending) {
		if (vcpu->arch.exception.nr == PF_VECTOR) {
			printk(KERN_DEBUG "kvm: inject_page_fault:"
					" double fault 0x%lx\n", addr);
			vcpu->arch.exception.nr = DF_VECTOR;
			vcpu->arch.exception.error_code = 0;
		} else if (vcpu->arch.exception.nr == DF_VECTOR) {
			/* triple fault -> shutdown */
			set_bit(KVM_REQ_TRIPLE_FAULT, &vcpu->requests);
		}
		return;
	}
	vcpu->arch.cr2 = addr;
	kvm_queue_exception_e(vcpu, PF_VECTOR, error_code);
}