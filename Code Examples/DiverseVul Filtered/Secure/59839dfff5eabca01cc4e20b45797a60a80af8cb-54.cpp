unsigned long realmode_get_cr(struct kvm_vcpu *vcpu, int cr)
{
	unsigned long value;

	kvm_x86_ops->decache_cr4_guest_bits(vcpu);
	switch (cr) {
	case 0:
		value = vcpu->arch.cr0;
		break;
	case 2:
		value = vcpu->arch.cr2;
		break;
	case 3:
		value = vcpu->arch.cr3;
		break;
	case 4:
		value = vcpu->arch.cr4;
		break;
	case 8:
		value = kvm_get_cr8(vcpu);
		break;
	default:
		vcpu_printf(vcpu, "%s: unexpected cr %u\n", __func__, cr);
		return 0;
	}
	KVMTRACE_3D(CR_READ, vcpu, (u32)cr, (u32)value,
		    (u32)((u64)value >> 32), handler);

	return value;
}