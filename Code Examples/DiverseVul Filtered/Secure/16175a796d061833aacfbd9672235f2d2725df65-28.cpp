static void enter_lmode(struct kvm_vcpu *vcpu)
{
	u32 guest_tr_ar;

	guest_tr_ar = vmcs_read32(GUEST_TR_AR_BYTES);
	if ((guest_tr_ar & AR_TYPE_MASK) != AR_TYPE_BUSY_64_TSS) {
		printk(KERN_DEBUG "%s: tss fixup for long mode. \n",
		       __func__);
		vmcs_write32(GUEST_TR_AR_BYTES,
			     (guest_tr_ar & ~AR_TYPE_MASK)
			     | AR_TYPE_BUSY_64_TSS);
	}
	vcpu->arch.shadow_efer |= EFER_LMA;
	vmx_set_efer(vcpu, vcpu->arch.shadow_efer);
}