static int vmx_check_intercept_io(struct kvm_vcpu *vcpu,
				  struct x86_instruction_info *info)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
	unsigned short port;
	bool intercept;
	int size;

	if (info->intercept == x86_intercept_in ||
	    info->intercept == x86_intercept_ins) {
		port = info->src_val;
		size = info->dst_bytes;
	} else {
		port = info->dst_val;
		size = info->src_bytes;
	}

	/*
	 * If the 'use IO bitmaps' VM-execution control is 0, IO instruction
	 * VM-exits depend on the 'unconditional IO exiting' VM-execution
	 * control.
	 *
	 * Otherwise, IO instruction VM-exits are controlled by the IO bitmaps.
	 */
	if (!nested_cpu_has(vmcs12, CPU_BASED_USE_IO_BITMAPS))
		intercept = nested_cpu_has(vmcs12,
					   CPU_BASED_UNCOND_IO_EXITING);
	else
		intercept = nested_vmx_check_io_bitmaps(vcpu, port, size);

	/* FIXME: produce nested vmexit and return X86EMUL_INTERCEPTED.  */
	return intercept ? X86EMUL_UNHANDLEABLE : X86EMUL_CONTINUE;
}