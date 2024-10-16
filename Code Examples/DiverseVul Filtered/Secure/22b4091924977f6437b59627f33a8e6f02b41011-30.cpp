static NTSTATUS smb_set_fsquota(connection_struct *conn,
			struct smb_request *req,
			files_struct *fsp,
			const DATA_BLOB *qdata)
{
	const struct loadparm_substitution *lp_sub =
		loadparm_s3_global_substitution();
	NTSTATUS status;
	SMB_NTQUOTA_STRUCT quotas;

	ZERO_STRUCT(quotas);

	/* access check */
	if ((get_current_uid(conn) != 0) || !CAN_WRITE(conn)) {
		DEBUG(3, ("set_fsquota: access_denied service [%s] user [%s]\n",
			  lp_servicename(talloc_tos(), lp_sub, SNUM(conn)),
			  conn->session_info->unix_info->unix_name));
		return NT_STATUS_ACCESS_DENIED;
	}

	if (!check_fsp_ntquota_handle(conn, req,
				      fsp)) {
		DEBUG(1, ("set_fsquota: no valid QUOTA HANDLE\n"));
		return NT_STATUS_INVALID_HANDLE;
	}

	/* note: normally there're 48 bytes,
	 * but we didn't use the last 6 bytes for now
	 * --metze
	 */
	if (qdata->length < 42) {
		DEBUG(0,("set_fsquota: requires total_data(%u) >= 42 bytes!\n",
			(unsigned int)qdata->length));
		return NT_STATUS_INVALID_PARAMETER;
	}

	/* unknown_1 24 NULL bytes in pdata*/

	/* the soft quotas 8 bytes (uint64_t)*/
	quotas.softlim = BVAL(qdata->data,24);

	/* the hard quotas 8 bytes (uint64_t)*/
	quotas.hardlim = BVAL(qdata->data,32);

	/* quota_flags 2 bytes **/
	quotas.qflags = SVAL(qdata->data,40);

	/* unknown_2 6 NULL bytes follow*/

	/* now set the quotas */
	if (vfs_set_ntquota(fsp, SMB_USER_FS_QUOTA_TYPE, NULL, &quotas)!=0) {
		DEBUG(1, ("vfs_set_ntquota() failed for service [%s]\n",
			  lp_servicename(talloc_tos(), lp_sub, SNUM(conn))));
		status =  map_nt_error_from_unix(errno);
	} else {
		status = NT_STATUS_OK;
	}
	return status;
}