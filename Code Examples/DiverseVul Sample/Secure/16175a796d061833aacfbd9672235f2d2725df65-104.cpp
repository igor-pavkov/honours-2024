static void setup_msrs(struct vcpu_vmx *vmx)
{
	int save_nmsrs;

	vmx_load_host_state(vmx);
	save_nmsrs = 0;
#ifdef CONFIG_X86_64
	if (is_long_mode(&vmx->vcpu)) {
		int index;

		index = __find_msr_index(vmx, MSR_SYSCALL_MASK);
		if (index >= 0)
			move_msr_up(vmx, index, save_nmsrs++);
		index = __find_msr_index(vmx, MSR_LSTAR);
		if (index >= 0)
			move_msr_up(vmx, index, save_nmsrs++);
		index = __find_msr_index(vmx, MSR_CSTAR);
		if (index >= 0)
			move_msr_up(vmx, index, save_nmsrs++);
		index = __find_msr_index(vmx, MSR_KERNEL_GS_BASE);
		if (index >= 0)
			move_msr_up(vmx, index, save_nmsrs++);
		/*
		 * MSR_K6_STAR is only needed on long mode guests, and only
		 * if efer.sce is enabled.
		 */
		index = __find_msr_index(vmx, MSR_K6_STAR);
		if ((index >= 0) && (vmx->vcpu.arch.shadow_efer & EFER_SCE))
			move_msr_up(vmx, index, save_nmsrs++);
	}
#endif
	vmx->save_nmsrs = save_nmsrs;

#ifdef CONFIG_X86_64
	vmx->msr_offset_kernel_gs_base =
		__find_msr_index(vmx, MSR_KERNEL_GS_BASE);
#endif
	vmx->msr_offset_efer = __find_msr_index(vmx, MSR_EFER);
}