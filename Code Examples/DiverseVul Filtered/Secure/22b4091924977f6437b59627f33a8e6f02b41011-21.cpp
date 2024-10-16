NTSTATUS smbd_do_setfsinfo(connection_struct *conn,
                                struct smb_request *req,
                                TALLOC_CTX *mem_ctx,
                                uint16_t info_level,
                                files_struct *fsp,
				const DATA_BLOB *pdata)
{
	switch (info_level) {
		case SMB_FS_QUOTA_INFORMATION:
		{
			return smb_set_fsquota(conn,
						req,
						fsp,
						pdata);
		}

		default:
			break;
	}
	return NT_STATUS_INVALID_LEVEL;
}