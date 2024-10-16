static __init void nested_vmx_setup_ctls_msrs(void)
{
	/*
	 * Note that as a general rule, the high half of the MSRs (bits in
	 * the control fields which may be 1) should be initialized by the
	 * intersection of the underlying hardware's MSR (i.e., features which
	 * can be supported) and the list of features we want to expose -
	 * because they are known to be properly supported in our code.
	 * Also, usually, the low half of the MSRs (bits which must be 1) can
	 * be set to 0, meaning that L1 may turn off any of these bits. The
	 * reason is that if one of these bits is necessary, it will appear
	 * in vmcs01 and prepare_vmcs02, when it bitwise-or's the control
	 * fields of vmcs01 and vmcs02, will turn these bits off - and
	 * nested_vmx_exit_handled() will not pass related exits to L1.
	 * These rules have exceptions below.
	 */

	/* pin-based controls */
	rdmsr(MSR_IA32_VMX_PINBASED_CTLS,
	      nested_vmx_pinbased_ctls_low, nested_vmx_pinbased_ctls_high);
	/*
	 * According to the Intel spec, if bit 55 of VMX_BASIC is off (as it is
	 * in our case), bits 1, 2 and 4 (i.e., 0x16) must be 1 in this MSR.
	 */
	nested_vmx_pinbased_ctls_low |= PIN_BASED_ALWAYSON_WITHOUT_TRUE_MSR;
	nested_vmx_pinbased_ctls_high &= PIN_BASED_EXT_INTR_MASK |
		PIN_BASED_NMI_EXITING | PIN_BASED_VIRTUAL_NMIS |
		PIN_BASED_VMX_PREEMPTION_TIMER;
	nested_vmx_pinbased_ctls_high |= PIN_BASED_ALWAYSON_WITHOUT_TRUE_MSR;

	/*
	 * Exit controls
	 * If bit 55 of VMX_BASIC is off, bits 0-8 and 10, 11, 13, 14, 16 and
	 * 17 must be 1.
	 */
	nested_vmx_exit_ctls_low = VM_EXIT_ALWAYSON_WITHOUT_TRUE_MSR;
	/* Note that guest use of VM_EXIT_ACK_INTR_ON_EXIT is not supported. */
#ifdef CONFIG_X86_64
	nested_vmx_exit_ctls_high = VM_EXIT_HOST_ADDR_SPACE_SIZE;
#else
	nested_vmx_exit_ctls_high = 0;
#endif
	nested_vmx_exit_ctls_high |= (VM_EXIT_ALWAYSON_WITHOUT_TRUE_MSR |
				      VM_EXIT_LOAD_IA32_EFER);

	/* entry controls */
	rdmsr(MSR_IA32_VMX_ENTRY_CTLS,
		nested_vmx_entry_ctls_low, nested_vmx_entry_ctls_high);
	/* If bit 55 of VMX_BASIC is off, bits 0-8 and 12 must be 1. */
	nested_vmx_entry_ctls_low = VM_ENTRY_ALWAYSON_WITHOUT_TRUE_MSR;
	nested_vmx_entry_ctls_high &=
		VM_ENTRY_LOAD_IA32_PAT | VM_ENTRY_IA32E_MODE;
	nested_vmx_entry_ctls_high |= (VM_ENTRY_ALWAYSON_WITHOUT_TRUE_MSR |
				       VM_ENTRY_LOAD_IA32_EFER);
	/* cpu-based controls */
	rdmsr(MSR_IA32_VMX_PROCBASED_CTLS,
		nested_vmx_procbased_ctls_low, nested_vmx_procbased_ctls_high);
	nested_vmx_procbased_ctls_low = 0;
	nested_vmx_procbased_ctls_high &=
		CPU_BASED_VIRTUAL_INTR_PENDING | CPU_BASED_USE_TSC_OFFSETING |
		CPU_BASED_HLT_EXITING | CPU_BASED_INVLPG_EXITING |
		CPU_BASED_MWAIT_EXITING | CPU_BASED_CR3_LOAD_EXITING |
		CPU_BASED_CR3_STORE_EXITING |
#ifdef CONFIG_X86_64
		CPU_BASED_CR8_LOAD_EXITING | CPU_BASED_CR8_STORE_EXITING |
#endif
		CPU_BASED_MOV_DR_EXITING | CPU_BASED_UNCOND_IO_EXITING |
		CPU_BASED_USE_IO_BITMAPS | CPU_BASED_MONITOR_EXITING |
		CPU_BASED_RDPMC_EXITING | CPU_BASED_RDTSC_EXITING |
		CPU_BASED_PAUSE_EXITING |
		CPU_BASED_ACTIVATE_SECONDARY_CONTROLS;
	/*
	 * We can allow some features even when not supported by the
	 * hardware. For example, L1 can specify an MSR bitmap - and we
	 * can use it to avoid exits to L1 - even when L0 runs L2
	 * without MSR bitmaps.
	 */
	nested_vmx_procbased_ctls_high |= CPU_BASED_USE_MSR_BITMAPS;

	/* secondary cpu-based controls */
	rdmsr(MSR_IA32_VMX_PROCBASED_CTLS2,
		nested_vmx_secondary_ctls_low, nested_vmx_secondary_ctls_high);
	nested_vmx_secondary_ctls_low = 0;
	nested_vmx_secondary_ctls_high &=
		SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES |
		SECONDARY_EXEC_WBINVD_EXITING;

	/* miscellaneous data */
	rdmsr(MSR_IA32_VMX_MISC, nested_vmx_misc_low, nested_vmx_misc_high);
	nested_vmx_misc_low &= VMX_MISC_PREEMPTION_TIMER_RATE_MASK |
		VMX_MISC_SAVE_EFER_LMA;
	nested_vmx_misc_high = 0;
}