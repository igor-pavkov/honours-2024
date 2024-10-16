static void nested_vmx_cr_fixed1_bits_update(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct kvm_cpuid_entry2 *entry;

	vmx->nested.msrs.cr0_fixed1 = 0xffffffff;
	vmx->nested.msrs.cr4_fixed1 = X86_CR4_PCE;

#define cr4_fixed1_update(_cr4_mask, _reg, _cpuid_mask) do {		\
	if (entry && (entry->_reg & (_cpuid_mask)))			\
		vmx->nested.msrs.cr4_fixed1 |= (_cr4_mask);	\
} while (0)

	entry = kvm_find_cpuid_entry(vcpu, 0x1, 0);
	cr4_fixed1_update(X86_CR4_VME,        edx, feature_bit(VME));
	cr4_fixed1_update(X86_CR4_PVI,        edx, feature_bit(VME));
	cr4_fixed1_update(X86_CR4_TSD,        edx, feature_bit(TSC));
	cr4_fixed1_update(X86_CR4_DE,         edx, feature_bit(DE));
	cr4_fixed1_update(X86_CR4_PSE,        edx, feature_bit(PSE));
	cr4_fixed1_update(X86_CR4_PAE,        edx, feature_bit(PAE));
	cr4_fixed1_update(X86_CR4_MCE,        edx, feature_bit(MCE));
	cr4_fixed1_update(X86_CR4_PGE,        edx, feature_bit(PGE));
	cr4_fixed1_update(X86_CR4_OSFXSR,     edx, feature_bit(FXSR));
	cr4_fixed1_update(X86_CR4_OSXMMEXCPT, edx, feature_bit(XMM));
	cr4_fixed1_update(X86_CR4_VMXE,       ecx, feature_bit(VMX));
	cr4_fixed1_update(X86_CR4_SMXE,       ecx, feature_bit(SMX));
	cr4_fixed1_update(X86_CR4_PCIDE,      ecx, feature_bit(PCID));
	cr4_fixed1_update(X86_CR4_OSXSAVE,    ecx, feature_bit(XSAVE));

	entry = kvm_find_cpuid_entry(vcpu, 0x7, 0);
	cr4_fixed1_update(X86_CR4_FSGSBASE,   ebx, feature_bit(FSGSBASE));
	cr4_fixed1_update(X86_CR4_SMEP,       ebx, feature_bit(SMEP));
	cr4_fixed1_update(X86_CR4_SMAP,       ebx, feature_bit(SMAP));
	cr4_fixed1_update(X86_CR4_PKE,        ecx, feature_bit(PKU));
	cr4_fixed1_update(X86_CR4_UMIP,       ecx, feature_bit(UMIP));
	cr4_fixed1_update(X86_CR4_LA57,       ecx, feature_bit(LA57));

#undef cr4_fixed1_update
}