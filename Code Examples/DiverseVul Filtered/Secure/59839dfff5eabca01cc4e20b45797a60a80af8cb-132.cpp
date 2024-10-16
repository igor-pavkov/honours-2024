static int emulator_write_emulated_onepage(unsigned long addr,
					   const void *val,
					   unsigned int bytes,
					   struct kvm_vcpu *vcpu)
{
	struct kvm_io_device *mmio_dev;
	gpa_t                 gpa;

	gpa = vcpu->arch.mmu.gva_to_gpa(vcpu, addr);

	if (gpa == UNMAPPED_GVA) {
		kvm_inject_page_fault(vcpu, addr, 2);
		return X86EMUL_PROPAGATE_FAULT;
	}

	/* For APIC access vmexit */
	if ((gpa & PAGE_MASK) == APIC_DEFAULT_PHYS_BASE)
		goto mmio;

	if (emulator_write_phys(vcpu, gpa, val, bytes))
		return X86EMUL_CONTINUE;

mmio:
	/*
	 * Is this MMIO handled locally?
	 */
	mutex_lock(&vcpu->kvm->lock);
	mmio_dev = vcpu_find_mmio_dev(vcpu, gpa, bytes, 1);
	if (mmio_dev) {
		kvm_iodevice_write(mmio_dev, gpa, bytes, val);
		mutex_unlock(&vcpu->kvm->lock);
		return X86EMUL_CONTINUE;
	}
	mutex_unlock(&vcpu->kvm->lock);

	vcpu->mmio_needed = 1;
	vcpu->mmio_phys_addr = gpa;
	vcpu->mmio_size = bytes;
	vcpu->mmio_is_write = 1;
	memcpy(vcpu->mmio_data, val, bytes);

	return X86EMUL_CONTINUE;
}