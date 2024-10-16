static int selinux_sem_associate(struct kern_ipc_perm *sma, int semflg)
{
	struct ipc_security_struct *isec;
	struct common_audit_data ad;
	u32 sid = current_sid();

	isec = selinux_ipc(sma);

	ad.type = LSM_AUDIT_DATA_IPC;
	ad.u.ipc_id = sma->key;

	return avc_has_perm(&selinux_state,
			    sid, isec->sid, SECCLASS_SEM,
			    SEM__ASSOCIATE, &ad);
}