static int kvm_get_msr_feature(struct kvm_msr_entry *msr)
{
	switch (msr->index) {
	case MSR_IA32_ARCH_CAPABILITIES:
		msr->data = kvm_get_arch_capabilities();
		break;
	case MSR_IA32_UCODE_REV:
		rdmsrl_safe(msr->index, &msr->data);
		break;
	default:
		if (kvm_x86_ops->get_msr_feature(msr))
			return 1;
	}
	return 0;
}