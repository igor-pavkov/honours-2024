static void vmx_vcpu_load(struct kvm_vcpu *vcpu, int cpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	vmx_vcpu_load_vmcs(vcpu, cpu, NULL);

	vmx_vcpu_pi_load(vcpu, cpu);

	vmx->host_debugctlmsr = get_debugctlmsr();
}