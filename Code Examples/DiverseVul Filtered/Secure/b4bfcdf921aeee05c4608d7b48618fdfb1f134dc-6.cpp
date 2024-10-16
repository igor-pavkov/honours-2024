NTSTATUS smbd_do_query_security_desc(connection_struct *conn,
					TALLOC_CTX *mem_ctx,
					files_struct *fsp,
					uint32_t security_info_wanted,
					uint32_t max_data_count,
					uint8_t **ppmarshalled_sd,
					size_t *psd_size)
{
	NTSTATUS status;
	struct security_descriptor *psd = NULL;
	TALLOC_CTX *frame = talloc_stackframe();

	/*
	 * Get the permissions to return.
	 */

	if ((security_info_wanted & SECINFO_SACL) &&
			!(fsp->access_mask & SEC_FLAG_SYSTEM_SECURITY)) {
		DEBUG(10, ("Access to SACL denied.\n"));
		TALLOC_FREE(frame);
		return NT_STATUS_ACCESS_DENIED;
	}

	if ((security_info_wanted & (SECINFO_DACL|SECINFO_OWNER|SECINFO_GROUP)) &&
			!(fsp->access_mask & SEC_STD_READ_CONTROL)) {
		DEBUG(10, ("Access to DACL, OWNER, or GROUP denied.\n"));
		TALLOC_FREE(frame);
		return NT_STATUS_ACCESS_DENIED;
	}

	if (security_info_wanted & (SECINFO_DACL|SECINFO_OWNER|
			SECINFO_GROUP|SECINFO_SACL)) {
		/* Don't return SECINFO_LABEL if anything else was
		   requested. See bug #8458. */
		security_info_wanted &= ~SECINFO_LABEL;
	}

	if (!lp_nt_acl_support(SNUM(conn))) {
		status = get_null_nt_acl(frame, &psd);
	} else if (security_info_wanted & SECINFO_LABEL) {
		/* Like W2K3 return a null object. */
		status = get_null_nt_acl(frame, &psd);
	} else {
		status = SMB_VFS_FGET_NT_ACL(
			fsp, security_info_wanted, frame, &psd);
	}
	if (!NT_STATUS_IS_OK(status)) {
		TALLOC_FREE(frame);
		return status;
	}

	if (!(security_info_wanted & SECINFO_OWNER)) {
		psd->owner_sid = NULL;
	}
	if (!(security_info_wanted & SECINFO_GROUP)) {
		psd->group_sid = NULL;
	}
	if (!(security_info_wanted & SECINFO_DACL)) {
		psd->type &= ~SEC_DESC_DACL_PRESENT;
		psd->dacl = NULL;
	}
	if (!(security_info_wanted & SECINFO_SACL)) {
		psd->type &= ~SEC_DESC_SACL_PRESENT;
		psd->sacl = NULL;
	}

	/* If the SACL/DACL is NULL, but was requested, we mark that it is
	 * present in the reply to match Windows behavior */
	if (psd->sacl == NULL &&
	    security_info_wanted & SECINFO_SACL)
		psd->type |= SEC_DESC_SACL_PRESENT;
	if (psd->dacl == NULL &&
	    security_info_wanted & SECINFO_DACL)
		psd->type |= SEC_DESC_DACL_PRESENT;

	if (security_info_wanted & SECINFO_LABEL) {
		/* Like W2K3 return a null object. */
		psd->owner_sid = NULL;
		psd->group_sid = NULL;
		psd->dacl = NULL;
		psd->sacl = NULL;
		psd->type &= ~(SEC_DESC_DACL_PRESENT|SEC_DESC_SACL_PRESENT);
	}

	*psd_size = ndr_size_security_descriptor(psd, 0);

	DEBUG(3,("smbd_do_query_security_desc: sd_size = %lu.\n",
		(unsigned long)*psd_size));

	if (DEBUGLEVEL >= 10) {
		DEBUG(10,("smbd_do_query_security_desc for file %s\n",
			  fsp_str_dbg(fsp)));
		NDR_PRINT_DEBUG(security_descriptor, psd);
	}

	if (max_data_count < *psd_size) {
		TALLOC_FREE(frame);
		return NT_STATUS_BUFFER_TOO_SMALL;
	}

	status = marshall_sec_desc(mem_ctx, psd,
				   ppmarshalled_sd, psd_size);

	if (!NT_STATUS_IS_OK(status)) {
		TALLOC_FREE(frame);
		return status;
	}

	TALLOC_FREE(frame);
	return NT_STATUS_OK;
}