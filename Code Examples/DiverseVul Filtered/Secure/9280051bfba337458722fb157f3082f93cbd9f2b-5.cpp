static NTSTATUS check_guest_password(auth_serversupplied_info **server_info)
{
	struct auth_context *auth_context;
	auth_usersupplied_info *user_info = NULL;

	NTSTATUS nt_status;
	unsigned char chal[8];

	ZERO_STRUCT(chal);

	DEBUG(3,("Got anonymous request\n"));

	if (!NT_STATUS_IS_OK(nt_status = make_auth_context_fixed(&auth_context,
					chal))) {
		return nt_status;
	}

	if (!make_user_info_guest(&user_info)) {
		(auth_context->free)(&auth_context);
		return NT_STATUS_NO_MEMORY;
	}

	nt_status = auth_context->check_ntlm_password(auth_context,
						user_info,
						server_info);
	(auth_context->free)(&auth_context);
	free_user_info(&user_info);
	return nt_status;
}