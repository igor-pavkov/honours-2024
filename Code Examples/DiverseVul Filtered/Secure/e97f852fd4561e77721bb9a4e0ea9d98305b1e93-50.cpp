int kvm_vm_ioctl_check_extension(struct kvm *kvm, long ext)
{
	int r = 0;

	switch (ext) {
	case KVM_CAP_IRQCHIP:
	case KVM_CAP_HLT:
	case KVM_CAP_MMU_SHADOW_CACHE_CONTROL:
	case KVM_CAP_SET_TSS_ADDR:
	case KVM_CAP_EXT_CPUID:
	case KVM_CAP_EXT_EMUL_CPUID:
	case KVM_CAP_CLOCKSOURCE:
	case KVM_CAP_PIT:
	case KVM_CAP_NOP_IO_DELAY:
	case KVM_CAP_MP_STATE:
	case KVM_CAP_SYNC_MMU:
	case KVM_CAP_USER_NMI:
	case KVM_CAP_REINJECT_CONTROL:
	case KVM_CAP_IRQ_INJECT_STATUS:
	case KVM_CAP_IOEVENTFD:
	case KVM_CAP_IOEVENTFD_NO_LENGTH:
	case KVM_CAP_PIT2:
	case KVM_CAP_PIT_STATE2:
	case KVM_CAP_SET_IDENTITY_MAP_ADDR:
	case KVM_CAP_XEN_HVM:
	case KVM_CAP_VCPU_EVENTS:
	case KVM_CAP_HYPERV:
	case KVM_CAP_HYPERV_VAPIC:
	case KVM_CAP_HYPERV_SPIN:
	case KVM_CAP_HYPERV_SYNIC:
	case KVM_CAP_HYPERV_SYNIC2:
	case KVM_CAP_HYPERV_VP_INDEX:
	case KVM_CAP_HYPERV_EVENTFD:
	case KVM_CAP_HYPERV_TLBFLUSH:
	case KVM_CAP_HYPERV_SEND_IPI:
	case KVM_CAP_HYPERV_ENLIGHTENED_VMCS:
	case KVM_CAP_PCI_SEGMENT:
	case KVM_CAP_DEBUGREGS:
	case KVM_CAP_X86_ROBUST_SINGLESTEP:
	case KVM_CAP_XSAVE:
	case KVM_CAP_ASYNC_PF:
	case KVM_CAP_GET_TSC_KHZ:
	case KVM_CAP_KVMCLOCK_CTRL:
	case KVM_CAP_READONLY_MEM:
	case KVM_CAP_HYPERV_TIME:
	case KVM_CAP_IOAPIC_POLARITY_IGNORED:
	case KVM_CAP_TSC_DEADLINE_TIMER:
	case KVM_CAP_ENABLE_CAP_VM:
	case KVM_CAP_DISABLE_QUIRKS:
	case KVM_CAP_SET_BOOT_CPU_ID:
 	case KVM_CAP_SPLIT_IRQCHIP:
	case KVM_CAP_IMMEDIATE_EXIT:
	case KVM_CAP_GET_MSR_FEATURES:
	case KVM_CAP_MSR_PLATFORM_INFO:
	case KVM_CAP_EXCEPTION_PAYLOAD:
		r = 1;
		break;
	case KVM_CAP_SYNC_REGS:
		r = KVM_SYNC_X86_VALID_FIELDS;
		break;
	case KVM_CAP_ADJUST_CLOCK:
		r = KVM_CLOCK_TSC_STABLE;
		break;
	case KVM_CAP_X86_DISABLE_EXITS:
		r |=  KVM_X86_DISABLE_EXITS_HLT | KVM_X86_DISABLE_EXITS_PAUSE;
		if(kvm_can_mwait_in_guest())
			r |= KVM_X86_DISABLE_EXITS_MWAIT;
		break;
	case KVM_CAP_X86_SMM:
		/* SMBASE is usually relocated above 1M on modern chipsets,
		 * and SMM handlers might indeed rely on 4G segment limits,
		 * so do not report SMM to be available if real mode is
		 * emulated via vm86 mode.  Still, do not go to great lengths
		 * to avoid userspace's usage of the feature, because it is a
		 * fringe case that is not enabled except via specific settings
		 * of the module parameters.
		 */
		r = kvm_x86_ops->has_emulated_msr(MSR_IA32_SMBASE);
		break;
	case KVM_CAP_VAPIC:
		r = !kvm_x86_ops->cpu_has_accelerated_tpr();
		break;
	case KVM_CAP_NR_VCPUS:
		r = KVM_SOFT_MAX_VCPUS;
		break;
	case KVM_CAP_MAX_VCPUS:
		r = KVM_MAX_VCPUS;
		break;
	case KVM_CAP_NR_MEMSLOTS:
		r = KVM_USER_MEM_SLOTS;
		break;
	case KVM_CAP_PV_MMU:	/* obsolete */
		r = 0;
		break;
	case KVM_CAP_MCE:
		r = KVM_MAX_MCE_BANKS;
		break;
	case KVM_CAP_XCRS:
		r = boot_cpu_has(X86_FEATURE_XSAVE);
		break;
	case KVM_CAP_TSC_CONTROL:
		r = kvm_has_tsc_control;
		break;
	case KVM_CAP_X2APIC_API:
		r = KVM_X2APIC_API_VALID_FLAGS;
		break;
	case KVM_CAP_NESTED_STATE:
		r = kvm_x86_ops->get_nested_state ?
			kvm_x86_ops->get_nested_state(NULL, 0, 0) : 0;
		break;
	default:
		break;
	}
	return r;

}