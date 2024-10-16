static void ipc_init_security(struct ipc_security_struct *isec, u16 sclass)
{
	isec->sclass = sclass;
	isec->sid = current_sid();
}