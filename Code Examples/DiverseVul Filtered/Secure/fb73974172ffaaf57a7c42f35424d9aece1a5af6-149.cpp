static int selinux_shm_associate(struct kern_ipc_perm *shp, int shmflg)
{
	struct ipc_security_struct *isec;
	struct common_audit_data ad;
	u32 sid = current_sid();

	isec = selinux_ipc(shp);

	ad.type = LSM_AUDIT_DATA_IPC;
	ad.u.ipc_id = shp->key;

	return avc_has_perm(&selinux_state,
			    sid, isec->sid, SECCLASS_SHM,
			    SHM__ASSOCIATE, &ad);
}