static int selinux_shm_alloc_security(struct kern_ipc_perm *shp)
{
	struct ipc_security_struct *isec;
	struct common_audit_data ad;
	u32 sid = current_sid();
	int rc;

	isec = selinux_ipc(shp);
	ipc_init_security(isec, SECCLASS_SHM);

	ad.type = LSM_AUDIT_DATA_IPC;
	ad.u.ipc_id = shp->key;

	rc = avc_has_perm(&selinux_state,
			  sid, isec->sid, SECCLASS_SHM,
			  SHM__CREATE, &ad);
	return rc;
}