static int selinux_msg_queue_associate(struct msg_queue *msq, int msqflg)
{
	struct ipc_security_struct *isec;
	struct common_audit_data ad;
	u32 sid = current_sid();

	isec = msq->q_perm.security;

	ad.type = LSM_AUDIT_DATA_IPC;
	ad.u.ipc_id = msq->q_perm.key;

	return avc_has_perm(sid, isec->sid, SECCLASS_MSGQ,
			    MSGQ__ASSOCIATE, &ad);
}