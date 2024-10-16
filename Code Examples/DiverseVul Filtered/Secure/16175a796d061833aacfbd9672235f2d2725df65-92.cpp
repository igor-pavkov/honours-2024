static int handle_cr(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	unsigned long exit_qualification;
	int cr;
	int reg;

	exit_qualification = vmcs_readl(EXIT_QUALIFICATION);
	cr = exit_qualification & 15;
	reg = (exit_qualification >> 8) & 15;
	switch ((exit_qualification >> 4) & 3) {
	case 0: /* mov to cr */
		KVMTRACE_3D(CR_WRITE, vcpu, (u32)cr,
			    (u32)kvm_register_read(vcpu, reg),
			    (u32)((u64)kvm_register_read(vcpu, reg) >> 32),
			    handler);
		switch (cr) {
		case 0:
			kvm_set_cr0(vcpu, kvm_register_read(vcpu, reg));
			skip_emulated_instruction(vcpu);
			return 1;
		case 3:
			kvm_set_cr3(vcpu, kvm_register_read(vcpu, reg));
			skip_emulated_instruction(vcpu);
			return 1;
		case 4:
			kvm_set_cr4(vcpu, kvm_register_read(vcpu, reg));
			skip_emulated_instruction(vcpu);
			return 1;
		case 8:
			kvm_set_cr8(vcpu, kvm_register_read(vcpu, reg));
			skip_emulated_instruction(vcpu);
			if (irqchip_in_kernel(vcpu->kvm))
				return 1;
			kvm_run->exit_reason = KVM_EXIT_SET_TPR;
			return 0;
		};
		break;
	case 2: /* clts */
		vmx_fpu_deactivate(vcpu);
		vcpu->arch.cr0 &= ~X86_CR0_TS;
		vmcs_writel(CR0_READ_SHADOW, vcpu->arch.cr0);
		vmx_fpu_activate(vcpu);
		KVMTRACE_0D(CLTS, vcpu, handler);
		skip_emulated_instruction(vcpu);
		return 1;
	case 1: /*mov from cr*/
		switch (cr) {
		case 3:
			kvm_register_write(vcpu, reg, vcpu->arch.cr3);
			KVMTRACE_3D(CR_READ, vcpu, (u32)cr,
				    (u32)kvm_register_read(vcpu, reg),
				    (u32)((u64)kvm_register_read(vcpu, reg) >> 32),
				    handler);
			skip_emulated_instruction(vcpu);
			return 1;
		case 8:
			kvm_register_write(vcpu, reg, kvm_get_cr8(vcpu));
			KVMTRACE_2D(CR_READ, vcpu, (u32)cr,
				    (u32)kvm_register_read(vcpu, reg), handler);
			skip_emulated_instruction(vcpu);
			return 1;
		}
		break;
	case 3: /* lmsw */
		kvm_lmsw(vcpu, (exit_qualification >> LMSW_SOURCE_DATA_SHIFT) & 0x0f);

		skip_emulated_instruction(vcpu);
		return 1;
	default:
		break;
	}
	kvm_run->exit_reason = 0;
	pr_unimpl(vcpu, "unhandled control register: op %d cr %d\n",
	       (int)(exit_qualification >> 4) & 3, cr);
	return 0;
}