void kvm_deliver_exception_payload(struct kvm_vcpu *vcpu)
{
	unsigned nr = vcpu->arch.exception.nr;
	bool has_payload = vcpu->arch.exception.has_payload;
	unsigned long payload = vcpu->arch.exception.payload;

	if (!has_payload)
		return;

	switch (nr) {
	case DB_VECTOR:
		/*
		 * "Certain debug exceptions may clear bit 0-3.  The
		 * remaining contents of the DR6 register are never
		 * cleared by the processor".
		 */
		vcpu->arch.dr6 &= ~DR_TRAP_BITS;
		/*
		 * DR6.RTM is set by all #DB exceptions that don't clear it.
		 */
		vcpu->arch.dr6 |= DR6_RTM;
		vcpu->arch.dr6 |= payload;
		/*
		 * Bit 16 should be set in the payload whenever the #DB
		 * exception should clear DR6.RTM. This makes the payload
		 * compatible with the pending debug exceptions under VMX.
		 * Though not currently documented in the SDM, this also
		 * makes the payload compatible with the exit qualification
		 * for #DB exceptions under VMX.
		 */
		vcpu->arch.dr6 ^= payload & DR6_RTM;
		break;
	case PF_VECTOR:
		vcpu->arch.cr2 = payload;
		break;
	}

	vcpu->arch.exception.has_payload = false;
	vcpu->arch.exception.payload = 0;
}