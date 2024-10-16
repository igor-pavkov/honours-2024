static void copy_vmcs12_to_shadow(struct vcpu_vmx *vmx)
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
	u64 field_value = 0;
	struct vmcs *shadow_vmcs = vmx->vmcs01.shadow_vmcs;

	vmcs_load(shadow_vmcs);

	for (q = 0; q < ARRAY_SIZE(fields); q++) {
		for (i = 0; i < max_fields[q]; i++) {
			field = fields[q][i];
			vmcs12_read_any(get_vmcs12(&vmx->vcpu), field, &field_value);
			__vmcs_writel(field, field_value);
		}
	}

	vmcs_clear(shadow_vmcs);
	vmcs_load(vmx->loaded_vmcs->vmcs);
}