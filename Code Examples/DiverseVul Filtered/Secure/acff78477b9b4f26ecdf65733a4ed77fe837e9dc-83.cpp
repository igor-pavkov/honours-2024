static u16 nested_get_vpid02(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	return vmx->nested.vpid02 ? vmx->nested.vpid02 : vmx->vpid;
}