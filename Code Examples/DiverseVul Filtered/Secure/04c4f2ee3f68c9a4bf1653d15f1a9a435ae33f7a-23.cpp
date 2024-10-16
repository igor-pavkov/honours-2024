static int vmx_check_intercept(struct kvm_vcpu *vcpu,
			       struct x86_instruction_info *info,
			       enum x86_intercept_stage stage,
			       struct x86_exception *exception)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);

	switch (info->intercept) {
	/*
	 * RDPID causes #UD if disabled through secondary execution controls.
	 * Because it is marked as EmulateOnUD, we need to intercept it here.
	 */
	case x86_intercept_rdtscp:
		if (!nested_cpu_has2(vmcs12, SECONDARY_EXEC_ENABLE_RDTSCP)) {
			exception->vector = UD_VECTOR;
			exception->error_code_valid = false;
			return X86EMUL_PROPAGATE_FAULT;
		}
		break;

	case x86_intercept_in:
	case x86_intercept_ins:
	case x86_intercept_out:
	case x86_intercept_outs:
		return vmx_check_intercept_io(vcpu, info);

	case x86_intercept_lgdt:
	case x86_intercept_lidt:
	case x86_intercept_lldt:
	case x86_intercept_ltr:
	case x86_intercept_sgdt:
	case x86_intercept_sidt:
	case x86_intercept_sldt:
	case x86_intercept_str:
		if (!nested_cpu_has2(vmcs12, SECONDARY_EXEC_DESC))
			return X86EMUL_CONTINUE;

		/* FIXME: produce nested vmexit and return X86EMUL_INTERCEPTED.  */
		break;

	/* TODO: check more intercepts... */
	default:
		break;
	}

	return X86EMUL_UNHANDLEABLE;
}