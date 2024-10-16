static int handle_ept_violation(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	u64 exit_qualification;
	gpa_t gpa;
	int gla_validity;

	exit_qualification = vmcs_read64(EXIT_QUALIFICATION);

	if (exit_qualification & (1 << 6)) {
		printk(KERN_ERR "EPT: GPA exceeds GAW!\n");
		return -ENOTSUPP;
	}

	gla_validity = (exit_qualification >> 7) & 0x3;
	if (gla_validity != 0x3 && gla_validity != 0x1 && gla_validity != 0) {
		printk(KERN_ERR "EPT: Handling EPT violation failed!\n");
		printk(KERN_ERR "EPT: GPA: 0x%lx, GVA: 0x%lx\n",
			(long unsigned int)vmcs_read64(GUEST_PHYSICAL_ADDRESS),
			(long unsigned int)vmcs_read64(GUEST_LINEAR_ADDRESS));
		printk(KERN_ERR "EPT: Exit qualification is 0x%lx\n",
			(long unsigned int)exit_qualification);
		kvm_run->exit_reason = KVM_EXIT_UNKNOWN;
		kvm_run->hw.hardware_exit_reason = 0;
		return -ENOTSUPP;
	}

	gpa = vmcs_read64(GUEST_PHYSICAL_ADDRESS);
	return kvm_mmu_page_fault(vcpu, gpa & PAGE_MASK, 0);
}