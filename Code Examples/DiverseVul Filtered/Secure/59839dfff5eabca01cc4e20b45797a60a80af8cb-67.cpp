static u32 get_tss_base_addr(struct kvm_vcpu *vcpu,
			     struct desc_struct *seg_desc)
{
	u32 base_addr;

	base_addr = seg_desc->base0;
	base_addr |= (seg_desc->base1 << 16);
	base_addr |= (seg_desc->base2 << 24);

	return vcpu->arch.mmu.gva_to_gpa(vcpu, base_addr);
}