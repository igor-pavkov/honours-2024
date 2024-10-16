static int emulator_cmpxchg_emulated(unsigned long addr,
				     const void *old,
				     const void *new,
				     unsigned int bytes,
				     struct kvm_vcpu *vcpu)
{
	static int reported;

	if (!reported) {
		reported = 1;
		printk(KERN_WARNING "kvm: emulating exchange as write\n");
	}
#ifndef CONFIG_X86_64
	/* guests cmpxchg8b have to be emulated atomically */
	if (bytes == 8) {
		gpa_t gpa;
		struct page *page;
		char *kaddr;
		u64 val;

		gpa = vcpu->arch.mmu.gva_to_gpa(vcpu, addr);

		if (gpa == UNMAPPED_GVA ||
		   (gpa & PAGE_MASK) == APIC_DEFAULT_PHYS_BASE)
			goto emul_write;

		if (((gpa + bytes - 1) & PAGE_MASK) != (gpa & PAGE_MASK))
			goto emul_write;

		val = *(u64 *)new;

		page = gfn_to_page(vcpu->kvm, gpa >> PAGE_SHIFT);

		kaddr = kmap_atomic(page, KM_USER0);
		set_64bit((u64 *)(kaddr + offset_in_page(gpa)), val);
		kunmap_atomic(kaddr, KM_USER0);
		kvm_release_page_dirty(page);
	}
emul_write:
#endif

	return emulator_write_emulated(addr, new, bytes, vcpu);
}