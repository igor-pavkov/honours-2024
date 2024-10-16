static void ept_load_pdptrs(struct kvm_vcpu *vcpu)
{
	if (is_paging(vcpu) && is_pae(vcpu) && !is_long_mode(vcpu)) {
		if (!load_pdptrs(vcpu, vcpu->arch.cr3)) {
			printk(KERN_ERR "EPT: Fail to load pdptrs!\n");
			return;
		}
		vmcs_write64(GUEST_PDPTR0, vcpu->arch.pdptrs[0]);
		vmcs_write64(GUEST_PDPTR1, vcpu->arch.pdptrs[1]);
		vmcs_write64(GUEST_PDPTR2, vcpu->arch.pdptrs[2]);
		vmcs_write64(GUEST_PDPTR3, vcpu->arch.pdptrs[3]);
	}
}