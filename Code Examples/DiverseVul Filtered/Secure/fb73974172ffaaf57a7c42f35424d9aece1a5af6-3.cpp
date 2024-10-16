static int selinux_getprocattr(struct task_struct *p,
			       char *name, char **value)
{
	const struct task_security_struct *__tsec;
	u32 sid;
	int error;
	unsigned len;

	rcu_read_lock();
	__tsec = selinux_cred(__task_cred(p));

	if (current != p) {
		error = avc_has_perm(&selinux_state,
				     current_sid(), __tsec->sid,
				     SECCLASS_PROCESS, PROCESS__GETATTR, NULL);
		if (error)
			goto bad;
	}

	if (!strcmp(name, "current"))
		sid = __tsec->sid;
	else if (!strcmp(name, "prev"))
		sid = __tsec->osid;
	else if (!strcmp(name, "exec"))
		sid = __tsec->exec_sid;
	else if (!strcmp(name, "fscreate"))
		sid = __tsec->create_sid;
	else if (!strcmp(name, "keycreate"))
		sid = __tsec->keycreate_sid;
	else if (!strcmp(name, "sockcreate"))
		sid = __tsec->sockcreate_sid;
	else {
		error = -EINVAL;
		goto bad;
	}
	rcu_read_unlock();

	if (!sid)
		return 0;

	error = security_sid_to_context(&selinux_state, sid, value, &len);
	if (error)
		return error;
	return len;

bad:
	rcu_read_unlock();
	return error;
}