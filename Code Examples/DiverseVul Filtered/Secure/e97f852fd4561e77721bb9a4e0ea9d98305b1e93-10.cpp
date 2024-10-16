static void kvm_vcpu_ioctl_x86_get_vcpu_events(struct kvm_vcpu *vcpu,
					       struct kvm_vcpu_events *events)
{
	process_nmi(vcpu);

	/*
	 * The API doesn't provide the instruction length for software
	 * exceptions, so don't report them. As long as the guest RIP
	 * isn't advanced, we should expect to encounter the exception
	 * again.
	 */
	if (kvm_exception_is_soft(vcpu->arch.exception.nr)) {
		events->exception.injected = 0;
		events->exception.pending = 0;
	} else {
		events->exception.injected = vcpu->arch.exception.injected;
		events->exception.pending = vcpu->arch.exception.pending;
		/*
		 * For ABI compatibility, deliberately conflate
		 * pending and injected exceptions when
		 * KVM_CAP_EXCEPTION_PAYLOAD isn't enabled.
		 */
		if (!vcpu->kvm->arch.exception_payload_enabled)
			events->exception.injected |=
				vcpu->arch.exception.pending;
	}
	events->exception.nr = vcpu->arch.exception.nr;
	events->exception.has_error_code = vcpu->arch.exception.has_error_code;
	events->exception.error_code = vcpu->arch.exception.error_code;
	events->exception_has_payload = vcpu->arch.exception.has_payload;
	events->exception_payload = vcpu->arch.exception.payload;

	events->interrupt.injected =
		vcpu->arch.interrupt.injected && !vcpu->arch.interrupt.soft;
	events->interrupt.nr = vcpu->arch.interrupt.nr;
	events->interrupt.soft = 0;
	events->interrupt.shadow = kvm_x86_ops->get_interrupt_shadow(vcpu);

	events->nmi.injected = vcpu->arch.nmi_injected;
	events->nmi.pending = vcpu->arch.nmi_pending != 0;
	events->nmi.masked = kvm_x86_ops->get_nmi_mask(vcpu);
	events->nmi.pad = 0;

	events->sipi_vector = 0; /* never valid when reporting to user space */

	events->smi.smm = is_smm(vcpu);
	events->smi.pending = vcpu->arch.smi_pending;
	events->smi.smm_inside_nmi =
		!!(vcpu->arch.hflags & HF_SMM_INSIDE_NMI_MASK);
	events->smi.latched_init = kvm_lapic_latched_init(vcpu);

	events->flags = (KVM_VCPUEVENT_VALID_NMI_PENDING
			 | KVM_VCPUEVENT_VALID_SHADOW
			 | KVM_VCPUEVENT_VALID_SMM);
	if (vcpu->kvm->arch.exception_payload_enabled)
		events->flags |= KVM_VCPUEVENT_VALID_PAYLOAD;

	memset(&events->reserved, 0, sizeof(events->reserved));
}