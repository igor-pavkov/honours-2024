int kvm_arch_vcpu_ioctl_translate(struct kvm_vcpu *vcpu,
				    struct kvm_translation *tr)
{
	unsigned long vaddr = tr->linear_address;
	gpa_t gpa;

	vcpu_load(vcpu);
	down_read(&vcpu->kvm->slots_lock);
	gpa = vcpu->arch.mmu.gva_to_gpa(vcpu, vaddr);
	up_read(&vcpu->kvm->slots_lock);
	tr->physical_address = gpa;
	tr->valid = gpa != UNMAPPED_GVA;
	tr->writeable = 1;
	tr->usermode = 0;
	vcpu_put(vcpu);

	return 0;
}