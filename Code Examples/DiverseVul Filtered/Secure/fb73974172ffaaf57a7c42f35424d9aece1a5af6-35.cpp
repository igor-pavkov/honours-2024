static int selinux_msg_msg_alloc_security(struct msg_msg *msg)
{
	struct msg_security_struct *msec;

	msec = selinux_msg_msg(msg);
	msec->sid = SECINITSID_UNLABELED;

	return 0;
}