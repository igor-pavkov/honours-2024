static void reset_bad_password_netlogon(TALLOC_CTX *mem_ctx,
					struct samba_kdc_db_context *kdc_db_ctx,
					struct netr_SendToSamBase *send_to_sam)
{
	struct dcerpc_binding_handle *irpc_handle;
	struct winbind_SendToSam req;

	irpc_handle = irpc_binding_handle_by_name(mem_ctx, kdc_db_ctx->msg_ctx,
						  "winbind_server",
						  &ndr_table_winbind);

	if (irpc_handle == NULL) {
		DEBUG(0, ("No winbind_server running!\n"));
		return;
	}

	req.in.message = *send_to_sam;

	dcerpc_winbind_SendToSam_r_send(mem_ctx, kdc_db_ctx->ev_ctx,
					irpc_handle, &req);
}