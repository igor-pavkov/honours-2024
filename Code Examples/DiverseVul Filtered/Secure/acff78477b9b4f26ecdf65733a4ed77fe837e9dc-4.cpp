static int handle_vmwrite(struct kvm_vcpu *vcpu)
{
	unsigned long field;
	gva_t gva;
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned long exit_qualification = vmcs_readl(EXIT_QUALIFICATION);
	u32 vmx_instruction_info = vmcs_read32(VMX_INSTRUCTION_INFO);

	/* The value to write might be 32 or 64 bits, depending on L1's long
	 * mode, and eventually we need to write that into a field of several
	 * possible lengths. The code below first zero-extends the value to 64
	 * bit (field_value), and then copies only the appropriate number of
	 * bits into the vmcs12 field.
	 */
	u64 field_value = 0;
	struct x86_exception e;
	struct vmcs12 *vmcs12;

	if (!nested_vmx_check_permission(vcpu))
		return 1;

	if (vmx->nested.current_vmptr == -1ull)
		return nested_vmx_failInvalid(vcpu);

	if (vmx_instruction_info & (1u << 10))
		field_value = kvm_register_readl(vcpu,
			(((vmx_instruction_info) >> 3) & 0xf));
	else {
		if (get_vmx_mem_address(vcpu, exit_qualification,
				vmx_instruction_info, false, &gva))
			return 1;
		if (kvm_read_guest_virt(vcpu, gva, &field_value,
					(is_64_bit_mode(vcpu) ? 8 : 4), &e)) {
			kvm_inject_page_fault(vcpu, &e);
			return 1;
		}
	}


	field = kvm_register_readl(vcpu, (((vmx_instruction_info) >> 28) & 0xf));
	/*
	 * If the vCPU supports "VMWRITE to any supported field in the
	 * VMCS," then the "read-only" fields are actually read/write.
	 */
	if (vmcs_field_readonly(field) &&
	    !nested_cpu_has_vmwrite_any_field(vcpu))
		return nested_vmx_failValid(vcpu,
			VMXERR_VMWRITE_READ_ONLY_VMCS_COMPONENT);

	if (!is_guest_mode(vcpu))
		vmcs12 = get_vmcs12(vcpu);
	else {
		/*
		 * When vmcs->vmcs_link_pointer is -1ull, any VMWRITE
		 * to shadowed-field sets the ALU flags for VMfailInvalid.
		 */
		if (get_vmcs12(vcpu)->vmcs_link_pointer == -1ull)
			return nested_vmx_failInvalid(vcpu);
		vmcs12 = get_shadow_vmcs12(vcpu);
	}

	if (vmcs12_write_any(vmcs12, field, field_value) < 0)
		return nested_vmx_failValid(vcpu,
			VMXERR_UNSUPPORTED_VMCS_COMPONENT);

	/*
	 * Do not track vmcs12 dirty-state if in guest-mode
	 * as we actually dirty shadow vmcs12 instead of vmcs12.
	 */
	if (!is_guest_mode(vcpu)) {
		switch (field) {
#define SHADOW_FIELD_RW(x) case x:
#include "vmcs_shadow_fields.h"
			/*
			 * The fields that can be updated by L1 without a vmexit are
			 * always updated in the vmcs02, the others go down the slow
			 * path of prepare_vmcs02.
			 */
			break;
		default:
			vmx->nested.dirty_vmcs12 = true;
			break;
		}
	}

	return nested_vmx_succeed(vcpu);
}