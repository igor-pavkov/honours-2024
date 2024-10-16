static void copy_shadow_to_vmcs12(struct vcpu_vmx *vmx)
{
	const u16 *fields[] = {
		shadow_read_write_fields,
		shadow_read_only_fields
	};
	const int max_fields[] = {
		max_shadow_read_write_fields,
		max_shadow_read_only_fields
	};
	int i, q;
	unsigned long field;
	u64 field_value;
	struct vmcs *shadow_vmcs = vmx->vmcs01.shadow_vmcs;

	preempt_disable();

	vmcs_load(shadow_vmcs);

	for (q = 0; q < ARRAY_SIZE(fields); q++) {
		for (i = 0; i < max_fields[q]; i++) {
			field = fields[q][i];
			field_value = __vmcs_readl(field);
			vmcs12_write_any(get_vmcs12(&vmx->vcpu), field, field_value);
		}
		/*
		 * Skip the VM-exit information fields if they are read-only.
		 */
		if (!nested_cpu_has_vmwrite_any_field(&vmx->vcpu))
			break;
	}

	vmcs_clear(shadow_vmcs);
	vmcs_load(vmx->loaded_vmcs->vmcs);

	preempt_enable();
}