void realmode_set_cr(struct kvm_vcpu *vcpu, int cr, unsigned long val,
		     unsigned long *rflags)
{
	KVMTRACE_3D(CR_WRITE, vcpu, (u32)cr, (u32)val,
		    (u32)((u64)val >> 32), handler);

	switch (cr) {
	case 0:
		kvm_set_cr0(vcpu, mk_cr_64(vcpu->arch.cr0, val));
		*rflags = kvm_x86_ops->get_rflags(vcpu);
		break;
	case 2:
		vcpu->arch.cr2 = val;
		break;
	case 3:
		kvm_set_cr3(vcpu, val);
		break;
	case 4:
		kvm_set_cr4(vcpu, mk_cr_64(vcpu->arch.cr4, val));
		break;
	case 8:
		kvm_set_cr8(vcpu, val & 0xfUL);
		break;
	default:
		vcpu_printf(vcpu, "%s: unexpected cr %u\n", __func__, cr);
	}
}