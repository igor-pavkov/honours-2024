static int selinux_capset(struct cred *new, const struct cred *old,
			  const kernel_cap_t *effective,
			  const kernel_cap_t *inheritable,
			  const kernel_cap_t *permitted)
{
	return avc_has_perm(&selinux_state,
			    cred_sid(old), cred_sid(new), SECCLASS_PROCESS,
			    PROCESS__SETCAP, NULL);
}