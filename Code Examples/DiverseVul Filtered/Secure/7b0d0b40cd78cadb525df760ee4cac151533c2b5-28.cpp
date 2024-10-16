static int selinux_shm_associate(struct shmid_kernel *shp, int shmflg)
{
	struct ipc_security_struct *isec;
	struct common_audit_data ad;
	u32 sid = current_sid();

	isec = shp->shm_perm.security;

	ad.type = LSM_AUDIT_DATA_IPC;
	ad.u.ipc_id = shp->shm_perm.key;

	return avc_has_perm(sid, isec->sid, SECCLASS_SHM,
			    SHM__ASSOCIATE, &ad);
}