static void call_nt_transact_get_user_quota(connection_struct *conn,
					    struct smb_request *req,
					    uint16 **ppsetup,
					    uint32 setup_count,
					    char **ppparams,
					    uint32 parameter_count,
					    char **ppdata,
					    uint32 data_count,
					    uint32 max_data_count)
{
	NTSTATUS nt_status = NT_STATUS_OK;
	char *params = *ppparams;
	char *pdata = *ppdata;
	char *entry;
	int data_len=0,param_len=0;
	int qt_len=0;
	int entry_len = 0;
	files_struct *fsp = NULL;
	uint16 level = 0;
	size_t sid_len;
	struct dom_sid sid;
	bool start_enum = True;
	SMB_NTQUOTA_STRUCT qt;
	SMB_NTQUOTA_LIST *tmp_list;
	SMB_NTQUOTA_HANDLE *qt_handle = NULL;

	ZERO_STRUCT(qt);

	/* access check */
	if (get_current_uid(conn) != 0) {
		DEBUG(1,("get_user_quota: access_denied service [%s] user "
			 "[%s]\n", lp_servicename(talloc_tos(), SNUM(conn)),
			 conn->session_info->unix_info->unix_name));
		reply_nterror(req, NT_STATUS_ACCESS_DENIED);
		return;
	}

	/*
	 * Ensure minimum number of parameters sent.
	 */

	if (parameter_count < 4) {
		DEBUG(0,("TRANSACT_GET_USER_QUOTA: requires %d >= 4 bytes parameters\n",parameter_count));
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	/* maybe we can check the quota_fnum */
	fsp = file_fsp(req, SVAL(params,0));
	if (!check_fsp_ntquota_handle(conn, req, fsp)) {
		DEBUG(3,("TRANSACT_GET_USER_QUOTA: no valid QUOTA HANDLE\n"));
		reply_nterror(req, NT_STATUS_INVALID_HANDLE);
		return;
	}

	/* the NULL pointer checking for fsp->fake_file_handle->pd
	 * is done by CHECK_NTQUOTA_HANDLE_OK()
	 */
	qt_handle = (SMB_NTQUOTA_HANDLE *)fsp->fake_file_handle->private_data;

	level = SVAL(params,2);

	/* unknown 12 bytes leading in params */

	switch (level) {
		case TRANSACT_GET_USER_QUOTA_LIST_CONTINUE:
			/* seems that we should continue with the enum here --metze */

			if (qt_handle->quota_list!=NULL &&
			    qt_handle->tmp_list==NULL) {

				/* free the list */
				free_ntquota_list(&(qt_handle->quota_list));

				/* Realloc the size of parameters and data we will return */
				param_len = 4;
				params = nttrans_realloc(ppparams, param_len);
				if(params == NULL) {
					reply_nterror(req, NT_STATUS_NO_MEMORY);
					return;
				}

				data_len = 0;
				SIVAL(params,0,data_len);

				break;
			}

			start_enum = False;

		case TRANSACT_GET_USER_QUOTA_LIST_START:

			if (qt_handle->quota_list==NULL &&
				qt_handle->tmp_list==NULL) {
				start_enum = True;
			}

			if (start_enum && vfs_get_user_ntquota_list(fsp,&(qt_handle->quota_list))!=0) {
				reply_nterror(req, NT_STATUS_INTERNAL_ERROR);
				return;
			}

			/* Realloc the size of parameters and data we will return */
			param_len = 4;
			params = nttrans_realloc(ppparams, param_len);
			if(params == NULL) {
				reply_nterror(req, NT_STATUS_NO_MEMORY);
				return;
			}

			/* we should not trust the value in max_data_count*/
			max_data_count = MIN(max_data_count,2048);

			pdata = nttrans_realloc(ppdata, max_data_count);/* should be max data count from client*/
			if(pdata == NULL) {
				reply_nterror(req, NT_STATUS_NO_MEMORY);
				return;
			}

			entry = pdata;

			/* set params Size of returned Quota Data 4 bytes*/
			/* but set it later when we know it */

			/* for each entry push the data */

			if (start_enum) {
				qt_handle->tmp_list = qt_handle->quota_list;
			}

			tmp_list = qt_handle->tmp_list;

			for (;((tmp_list!=NULL)&&((qt_len +40+SID_MAX_SIZE)<max_data_count));
				tmp_list=tmp_list->next,entry+=entry_len,qt_len+=entry_len) {

				sid_len = ndr_size_dom_sid(
					&tmp_list->quotas->sid, 0);
				entry_len = 40 + sid_len;

				/* nextoffset entry 4 bytes */
				SIVAL(entry,0,entry_len);

				/* then the len of the SID 4 bytes */
				SIVAL(entry,4,sid_len);

				/* unknown data 8 bytes uint64_t */
				SBIG_UINT(entry,8,(uint64_t)0); /* this is not 0 in windows...-metze*/

				/* the used disk space 8 bytes uint64_t */
				SBIG_UINT(entry,16,tmp_list->quotas->usedspace);

				/* the soft quotas 8 bytes uint64_t */
				SBIG_UINT(entry,24,tmp_list->quotas->softlim);

				/* the hard quotas 8 bytes uint64_t */
				SBIG_UINT(entry,32,tmp_list->quotas->hardlim);

				/* and now the SID */
				sid_linearize(entry+40, sid_len, &tmp_list->quotas->sid);
			}

			qt_handle->tmp_list = tmp_list;

			/* overwrite the offset of the last entry */
			SIVAL(entry-entry_len,0,0);

			data_len = 4+qt_len;
			/* overwrite the params quota_data_len */
			SIVAL(params,0,data_len);

			break;

		case TRANSACT_GET_USER_QUOTA_FOR_SID:

			/* unknown 4 bytes IVAL(pdata,0) */

			if (data_count < 8) {
				DEBUG(0,("TRANSACT_GET_USER_QUOTA_FOR_SID: requires %d >= %d bytes data\n",data_count,8));
				reply_nterror(req, NT_STATUS_INVALID_LEVEL);
				return;
			}

			sid_len = IVAL(pdata,4);
			/* Ensure this is less than 1mb. */
			if (sid_len > (1024*1024)) {
				reply_nterror(req, NT_STATUS_NO_MEMORY);
				return;
			}

			if (data_count < 8+sid_len) {
				DEBUG(0,("TRANSACT_GET_USER_QUOTA_FOR_SID: requires %d >= %lu bytes data\n",data_count,(unsigned long)(8+sid_len)));
				reply_nterror(req, NT_STATUS_INVALID_LEVEL);
				return;
			}

			data_len = 4+40+sid_len;

			if (max_data_count < data_len) {
				DEBUG(0,("TRANSACT_GET_USER_QUOTA_FOR_SID: max_data_count(%d) < data_len(%d)\n",
					max_data_count, data_len));
				param_len = 4;
				SIVAL(params,0,data_len);
				data_len = 0;
				nt_status = NT_STATUS_BUFFER_TOO_SMALL;
				break;
			}

			if (!sid_parse(pdata+8,sid_len,&sid)) {
				reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
				return;
			}

			if (vfs_get_ntquota(fsp, SMB_USER_QUOTA_TYPE, &sid, &qt)!=0) {
				ZERO_STRUCT(qt);
				/*
				 * we have to return zero's in all fields
				 * instead of returning an error here
				 * --metze
				 */
			}

			/* Realloc the size of parameters and data we will return */
			param_len = 4;
			params = nttrans_realloc(ppparams, param_len);
			if(params == NULL) {
				reply_nterror(req, NT_STATUS_NO_MEMORY);
				return;
			}

			pdata = nttrans_realloc(ppdata, data_len);
			if(pdata == NULL) {
				reply_nterror(req, NT_STATUS_NO_MEMORY);
				return;
			}

			entry = pdata;

			/* set params Size of returned Quota Data 4 bytes*/
			SIVAL(params,0,data_len);

			/* nextoffset entry 4 bytes */
			SIVAL(entry,0,0);

			/* then the len of the SID 4 bytes */
			SIVAL(entry,4,sid_len);

			/* unknown data 8 bytes uint64_t */
			SBIG_UINT(entry,8,(uint64_t)0); /* this is not 0 in windows...-mezte*/

			/* the used disk space 8 bytes uint64_t */
			SBIG_UINT(entry,16,qt.usedspace);

			/* the soft quotas 8 bytes uint64_t */
			SBIG_UINT(entry,24,qt.softlim);

			/* the hard quotas 8 bytes uint64_t */
			SBIG_UINT(entry,32,qt.hardlim);

			/* and now the SID */
			sid_linearize(entry+40, sid_len, &sid);

			break;

		default:
			DEBUG(0, ("do_nt_transact_get_user_quota: %s: unknown "
				  "level 0x%04hX\n",
				  fsp_fnum_dbg(fsp), level));
			reply_nterror(req, NT_STATUS_INVALID_LEVEL);
			return;
			break;
	}

	send_nt_replies(conn, req, nt_status, params, param_len,
			pdata, data_len);
}