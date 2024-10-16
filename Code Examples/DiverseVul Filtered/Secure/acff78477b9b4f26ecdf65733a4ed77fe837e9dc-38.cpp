static int nested_vmx_check_exception(struct kvm_vcpu *vcpu, unsigned long *exit_qual)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
	unsigned int nr = vcpu->arch.exception.nr;
	bool has_payload = vcpu->arch.exception.has_payload;
	unsigned long payload = vcpu->arch.exception.payload;

	if (nr == PF_VECTOR) {
		if (vcpu->arch.exception.nested_apf) {
			*exit_qual = vcpu->arch.apf.nested_apf_token;
			return 1;
		}
		if (nested_vmx_is_page_fault_vmexit(vmcs12,
						    vcpu->arch.exception.error_code)) {
			*exit_qual = has_payload ? payload : vcpu->arch.cr2;
			return 1;
		}
	} else if (vmcs12->exception_bitmap & (1u << nr)) {
		if (nr == DB_VECTOR) {
			if (!has_payload) {
				payload = vcpu->arch.dr6;
				payload &= ~(DR6_FIXED_1 | DR6_BT);
				payload ^= DR6_RTM;
			}
			*exit_qual = payload;
		} else
			*exit_qual = 0;
		return 1;
	}

	return 0;
}