static int selinux_sem_associate(struct sem_array *sma, int semflg)
{
	struct ipc_security_struct *isec;
	struct common_audit_data ad;
	u32 sid = current_sid();

	isec = sma->sem_perm.security;

	ad.type = LSM_AUDIT_DATA_IPC;
	ad.u.ipc_id = sma->sem_perm.key;

	return avc_has_perm(sid, isec->sid, SECCLASS_SEM,
			    SEM__ASSOCIATE, &ad);
}