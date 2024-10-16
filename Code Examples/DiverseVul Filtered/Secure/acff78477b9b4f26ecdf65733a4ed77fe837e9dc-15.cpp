__init int nested_vmx_hardware_setup(int (*exit_handlers[])(struct kvm_vcpu *))
{
	int i;

	if (!cpu_has_vmx_shadow_vmcs())
		enable_shadow_vmcs = 0;
	if (enable_shadow_vmcs) {
		for (i = 0; i < VMX_BITMAP_NR; i++) {
			/*
			 * The vmx_bitmap is not tied to a VM and so should
			 * not be charged to a memcg.
			 */
			vmx_bitmap[i] = (unsigned long *)
				__get_free_page(GFP_KERNEL);
			if (!vmx_bitmap[i]) {
				nested_vmx_hardware_unsetup();
				return -ENOMEM;
			}
		}

		init_vmcs_shadow_fields();
	}

	exit_handlers[EXIT_REASON_VMCLEAR]	= handle_vmclear,
	exit_handlers[EXIT_REASON_VMLAUNCH]	= handle_vmlaunch,
	exit_handlers[EXIT_REASON_VMPTRLD]	= handle_vmptrld,
	exit_handlers[EXIT_REASON_VMPTRST]	= handle_vmptrst,
	exit_handlers[EXIT_REASON_VMREAD]	= handle_vmread,
	exit_handlers[EXIT_REASON_VMRESUME]	= handle_vmresume,
	exit_handlers[EXIT_REASON_VMWRITE]	= handle_vmwrite,
	exit_handlers[EXIT_REASON_VMOFF]	= handle_vmoff,
	exit_handlers[EXIT_REASON_VMON]		= handle_vmon,
	exit_handlers[EXIT_REASON_INVEPT]	= handle_invept,
	exit_handlers[EXIT_REASON_INVVPID]	= handle_invvpid,
	exit_handlers[EXIT_REASON_VMFUNC]	= handle_vmfunc,

	kvm_x86_ops->check_nested_events = vmx_check_nested_events;
	kvm_x86_ops->get_nested_state = vmx_get_nested_state;
	kvm_x86_ops->set_nested_state = vmx_set_nested_state;
	kvm_x86_ops->get_vmcs12_pages = nested_get_vmcs12_pages,
	kvm_x86_ops->nested_enable_evmcs = nested_enable_evmcs;
	kvm_x86_ops->nested_get_evmcs_version = nested_get_evmcs_version;

	return 0;
}