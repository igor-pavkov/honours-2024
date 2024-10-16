static int selinux_sem_alloc_security(struct sem_array *sma)
{
	struct ipc_security_struct *isec;
	struct common_audit_data ad;
	u32 sid = current_sid();
	int rc;

	rc = ipc_alloc_security(current, &sma->sem_perm, SECCLASS_SEM);
	if (rc)
		return rc;

	isec = sma->sem_perm.security;

	ad.type = LSM_AUDIT_DATA_IPC;
	ad.u.ipc_id = sma->sem_perm.key;

	rc = avc_has_perm(sid, isec->sid, SECCLASS_SEM,
			  SEM__CREATE, &ad);
	if (rc) {
		ipc_free_security(&sma->sem_perm);
		return rc;
	}
	return 0;
}