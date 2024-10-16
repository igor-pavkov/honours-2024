static int selinux_sem_alloc_security(struct kern_ipc_perm *sma)
{
	struct ipc_security_struct *isec;
	struct common_audit_data ad;
	u32 sid = current_sid();
	int rc;

	isec = selinux_ipc(sma);
	ipc_init_security(isec, SECCLASS_SEM);

	ad.type = LSM_AUDIT_DATA_IPC;
	ad.u.ipc_id = sma->key;

	rc = avc_has_perm(&selinux_state,
			  sid, isec->sid, SECCLASS_SEM,
			  SEM__CREATE, &ad);
	return rc;
}