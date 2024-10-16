static void vmx_setup_mce(struct kvm_vcpu *vcpu)
{
	if (vcpu->arch.mcg_cap & MCG_LMCE_P)
		to_vmx(vcpu)->msr_ia32_feature_control_valid_bits |=
			FEAT_CTL_LMCE_ENABLED;
	else
		to_vmx(vcpu)->msr_ia32_feature_control_valid_bits &=
			~FEAT_CTL_LMCE_ENABLED;
}