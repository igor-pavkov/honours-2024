static void kvmppc_complete_mmio_load(struct kvm_vcpu *vcpu,
                                      struct kvm_run *run)
{
	u64 uninitialized_var(gpr);

	if (run->mmio.len > sizeof(gpr)) {
		printk(KERN_ERR "bad MMIO length: %d\n", run->mmio.len);
		return;
	}

	if (!vcpu->arch.mmio_host_swabbed) {
		switch (run->mmio.len) {
		case 8: gpr = *(u64 *)run->mmio.data; break;
		case 4: gpr = *(u32 *)run->mmio.data; break;
		case 2: gpr = *(u16 *)run->mmio.data; break;
		case 1: gpr = *(u8 *)run->mmio.data; break;
		}
	} else {
		switch (run->mmio.len) {
		case 8: gpr = swab64(*(u64 *)run->mmio.data); break;
		case 4: gpr = swab32(*(u32 *)run->mmio.data); break;
		case 2: gpr = swab16(*(u16 *)run->mmio.data); break;
		case 1: gpr = *(u8 *)run->mmio.data; break;
		}
	}

	/* conversion between single and double precision */
	if ((vcpu->arch.mmio_sp64_extend) && (run->mmio.len == 4))
		gpr = sp_to_dp(gpr);

	if (vcpu->arch.mmio_sign_extend) {
		switch (run->mmio.len) {
#ifdef CONFIG_PPC64
		case 4:
			gpr = (s64)(s32)gpr;
			break;
#endif
		case 2:
			gpr = (s64)(s16)gpr;
			break;
		case 1:
			gpr = (s64)(s8)gpr;
			break;
		}
	}

	switch (vcpu->arch.io_gpr & KVM_MMIO_REG_EXT_MASK) {
	case KVM_MMIO_REG_GPR:
		kvmppc_set_gpr(vcpu, vcpu->arch.io_gpr, gpr);
		break;
	case KVM_MMIO_REG_FPR:
		VCPU_FPR(vcpu, vcpu->arch.io_gpr & KVM_MMIO_REG_MASK) = gpr;
		break;
#ifdef CONFIG_PPC_BOOK3S
	case KVM_MMIO_REG_QPR:
		vcpu->arch.qpr[vcpu->arch.io_gpr & KVM_MMIO_REG_MASK] = gpr;
		break;
	case KVM_MMIO_REG_FQPR:
		VCPU_FPR(vcpu, vcpu->arch.io_gpr & KVM_MMIO_REG_MASK) = gpr;
		vcpu->arch.qpr[vcpu->arch.io_gpr & KVM_MMIO_REG_MASK] = gpr;
		break;
#endif
#ifdef CONFIG_VSX
	case KVM_MMIO_REG_VSX:
		if (vcpu->arch.mmio_vsx_copy_type == KVMPPC_VSX_COPY_DWORD)
			kvmppc_set_vsr_dword(vcpu, gpr);
		else if (vcpu->arch.mmio_vsx_copy_type == KVMPPC_VSX_COPY_WORD)
			kvmppc_set_vsr_word(vcpu, gpr);
		else if (vcpu->arch.mmio_vsx_copy_type ==
				KVMPPC_VSX_COPY_DWORD_LOAD_DUMP)
			kvmppc_set_vsr_dword_dump(vcpu, gpr);
		break;
#endif
	default:
		BUG();
	}
}