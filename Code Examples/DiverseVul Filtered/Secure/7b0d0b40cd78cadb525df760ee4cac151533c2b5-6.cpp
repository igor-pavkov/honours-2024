static int selinux_msg_queue_alloc_security(struct msg_queue *msq)
{
	struct ipc_security_struct *isec;
	struct common_audit_data ad;
	u32 sid = current_sid();
	int rc;

	rc = ipc_alloc_security(current, &msq->q_perm, SECCLASS_MSGQ);
	if (rc)
		return rc;

	isec = msq->q_perm.security;

	ad.type = LSM_AUDIT_DATA_IPC;
	ad.u.ipc_id = msq->q_perm.key;

	rc = avc_has_perm(sid, isec->sid, SECCLASS_MSGQ,
			  MSGQ__CREATE, &ad);
	if (rc) {
		ipc_free_security(&msq->q_perm);
		return rc;
	}
	return 0;
}