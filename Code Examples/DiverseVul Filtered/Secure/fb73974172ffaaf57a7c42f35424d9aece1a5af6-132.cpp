static int selinux_kernel_module_request(char *kmod_name)
{
	struct common_audit_data ad;

	ad.type = LSM_AUDIT_DATA_KMOD;
	ad.u.kmod_name = kmod_name;

	return avc_has_perm(&selinux_state,
			    current_sid(), SECINITSID_KERNEL, SECCLASS_SYSTEM,
			    SYSTEM__MODULE_REQUEST, &ad);
}