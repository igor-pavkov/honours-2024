NTSTATUS smbd_do_qfsinfo(struct smbXsrv_connection *xconn,
			 connection_struct *conn,
			 TALLOC_CTX *mem_ctx,
			 uint16_t info_level,
			 uint16_t flags2,
			 unsigned int max_data_bytes,
			 size_t *fixed_portion,
			 struct smb_filename *fname,
			 char **ppdata,
			 int *ret_data_len)
{
	const struct loadparm_substitution *lp_sub =
		loadparm_s3_global_substitution();
	char *pdata, *end_data;
	int data_len = 0;
	size_t len = 0;
	const char *vname = volume_label(talloc_tos(), SNUM(conn));
	int snum = SNUM(conn);
	const char *fstype = lp_fstype(SNUM(conn));
	const char *filename = NULL;
	const uint64_t bytes_per_sector = 512;
	uint32_t additional_flags = 0;
	struct smb_filename smb_fname;
	SMB_STRUCT_STAT st;
	NTSTATUS status = NT_STATUS_OK;
	uint64_t df_ret;

	if (fname == NULL || fname->base_name == NULL) {
		filename = ".";
	} else {
		filename = fname->base_name;
	}

	if (IS_IPC(conn)) {
		if (info_level != SMB_QUERY_CIFS_UNIX_INFO) {
			DEBUG(0,("smbd_do_qfsinfo: not an allowed "
				"info level (0x%x) on IPC$.\n",
				(unsigned int)info_level));
			return NT_STATUS_ACCESS_DENIED;
		}
	}

	DEBUG(3,("smbd_do_qfsinfo: level = %d\n", info_level));

	smb_fname = (struct smb_filename) {
		.base_name = discard_const_p(char, filename),
		.flags = fname ? fname->flags : 0,
		.twrp = fname ? fname->twrp : 0,
	};

	if(info_level != SMB_FS_QUOTA_INFORMATION
	   && SMB_VFS_STAT(conn, &smb_fname) != 0) {
		DEBUG(2,("stat of . failed (%s)\n", strerror(errno)));
		return map_nt_error_from_unix(errno);
	}

	st = smb_fname.st;

	if (max_data_bytes + DIR_ENTRY_SAFETY_MARGIN < max_data_bytes) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	*ppdata = (char *)SMB_REALLOC(
		*ppdata, max_data_bytes + DIR_ENTRY_SAFETY_MARGIN);
	if (*ppdata == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	pdata = *ppdata;
	memset((char *)pdata,'\0',max_data_bytes + DIR_ENTRY_SAFETY_MARGIN);
	end_data = pdata + max_data_bytes + DIR_ENTRY_SAFETY_MARGIN - 1;

	*fixed_portion = 0;

	switch (info_level) {
		case SMB_INFO_ALLOCATION:
		{
			uint64_t dfree,dsize,bsize,block_size,sectors_per_unit;
			data_len = 18;
			df_ret = get_dfree_info(conn, &smb_fname, &bsize,
						&dfree, &dsize);
			if (df_ret == (uint64_t)-1) {
				return map_nt_error_from_unix(errno);
			}

			block_size = lp_block_size(snum);
			if (bsize < block_size) {
				uint64_t factor = block_size/bsize;
				bsize = block_size;
				dsize /= factor;
				dfree /= factor;
			}
			if (bsize > block_size) {
				uint64_t factor = bsize/block_size;
				bsize = block_size;
				dsize *= factor;
				dfree *= factor;
			}
			sectors_per_unit = bsize/bytes_per_sector;

			DEBUG(5,("smbd_do_qfsinfo : SMB_INFO_ALLOCATION id=%x, bsize=%u, cSectorUnit=%u, \
cBytesSector=%u, cUnitTotal=%u, cUnitAvail=%d\n", (unsigned int)st.st_ex_dev, (unsigned int)bsize, (unsigned int)sectors_per_unit,
				(unsigned int)bytes_per_sector, (unsigned int)dsize, (unsigned int)dfree));

			/*
			 * For large drives, return max values and not modulo.
			 */
			dsize = MIN(dsize, UINT32_MAX);
			dfree = MIN(dfree, UINT32_MAX);

			SIVAL(pdata,l1_idFileSystem,st.st_ex_dev);
			SIVAL(pdata,l1_cSectorUnit,sectors_per_unit);
			SIVAL(pdata,l1_cUnit,dsize);
			SIVAL(pdata,l1_cUnitAvail,dfree);
			SSVAL(pdata,l1_cbSector,bytes_per_sector);
			break;
		}

		case SMB_INFO_VOLUME:
			/* Return volume name */
			/* 
			 * Add volume serial number - hash of a combination of
			 * the called hostname and the service name.
			 */
			SIVAL(pdata,0,str_checksum(lp_servicename(talloc_tos(), lp_sub, snum)) ^ (str_checksum(get_local_machine_name())<<16) );
			/*
			 * Win2k3 and previous mess this up by sending a name length
			 * one byte short. I believe only older clients (OS/2 Win9x) use
			 * this call so try fixing this by adding a terminating null to
			 * the pushed string. The change here was adding the STR_TERMINATE. JRA.
			 */
			status = srvstr_push(
				pdata, flags2,
				pdata+l2_vol_szVolLabel, vname,
				PTR_DIFF(end_data, pdata+l2_vol_szVolLabel),
				STR_NOALIGN|STR_TERMINATE, &len);
			if (!NT_STATUS_IS_OK(status)) {
				return status;
			}
			SCVAL(pdata,l2_vol_cch,len);
			data_len = l2_vol_szVolLabel + len;
			DEBUG(5,("smbd_do_qfsinfo : time = %x, namelen = %u, name = %s\n",
				 (unsigned)convert_timespec_to_time_t(st.st_ex_ctime),
				 (unsigned)len, vname));
			break;

		case SMB_QUERY_FS_ATTRIBUTE_INFO:
		case SMB_FS_ATTRIBUTE_INFORMATION:

			additional_flags = 0;
#if defined(HAVE_SYS_QUOTAS)
			additional_flags |= FILE_VOLUME_QUOTAS;
#endif

			if(lp_nt_acl_support(SNUM(conn))) {
				additional_flags |= FILE_PERSISTENT_ACLS;
			}

			/* Capabilities are filled in at connection time through STATVFS call */
			additional_flags |= conn->fs_capabilities;
			additional_flags |= lp_parm_int(conn->params->service,
							"share", "fake_fscaps",
							0);

			SIVAL(pdata,0,FILE_CASE_PRESERVED_NAMES|FILE_CASE_SENSITIVE_SEARCH|
				FILE_SUPPORTS_OBJECT_IDS|FILE_UNICODE_ON_DISK|
				additional_flags); /* FS ATTRIBUTES */

			SIVAL(pdata,4,255); /* Max filename component length */
			/* NOTE! the fstype must *not* be null terminated or win98 won't recognise it
				and will think we can't do long filenames */
			status = srvstr_push(pdata, flags2, pdata+12, fstype,
					  PTR_DIFF(end_data, pdata+12),
					  STR_UNICODE, &len);
			if (!NT_STATUS_IS_OK(status)) {
				return status;
			}
			SIVAL(pdata,8,len);
			data_len = 12 + len;
			if (max_data_bytes >= 16 && data_len > max_data_bytes) {
				/* the client only requested a portion of the
				   file system name */
				data_len = max_data_bytes;
				status = STATUS_BUFFER_OVERFLOW;
			}
			*fixed_portion = 16;
			break;

		case SMB_QUERY_FS_LABEL_INFO:
		case SMB_FS_LABEL_INFORMATION:
			status = srvstr_push(pdata, flags2, pdata+4, vname,
					  PTR_DIFF(end_data, pdata+4), 0, &len);
			if (!NT_STATUS_IS_OK(status)) {
				return status;
			}
			data_len = 4 + len;
			SIVAL(pdata,0,len);
			break;

		case SMB_QUERY_FS_VOLUME_INFO:      
		case SMB_FS_VOLUME_INFORMATION:

			/* 
			 * Add volume serial number - hash of a combination of
			 * the called hostname and the service name.
			 */
			SIVAL(pdata,8,str_checksum(lp_servicename(talloc_tos(), lp_sub, snum)) ^
				(str_checksum(get_local_machine_name())<<16));

			/* Max label len is 32 characters. */
			status = srvstr_push(pdata, flags2, pdata+18, vname,
					  PTR_DIFF(end_data, pdata+18),
					  STR_UNICODE, &len);
			if (!NT_STATUS_IS_OK(status)) {
				return status;
			}
			SIVAL(pdata,12,len);
			data_len = 18+len;

			DEBUG(5,("smbd_do_qfsinfo : SMB_QUERY_FS_VOLUME_INFO namelen = %d, vol=%s serv=%s\n",
				(int)strlen(vname),vname,
				lp_servicename(talloc_tos(), lp_sub, snum)));
			if (max_data_bytes >= 24 && data_len > max_data_bytes) {
				/* the client only requested a portion of the
				   volume label */
				data_len = max_data_bytes;
				status = STATUS_BUFFER_OVERFLOW;
			}
			*fixed_portion = 24;
			break;

		case SMB_QUERY_FS_SIZE_INFO:
		case SMB_FS_SIZE_INFORMATION:
		{
			uint64_t dfree,dsize,bsize,block_size,sectors_per_unit;
			data_len = 24;
			df_ret = get_dfree_info(conn, &smb_fname, &bsize,
						&dfree, &dsize);
			if (df_ret == (uint64_t)-1) {
				return map_nt_error_from_unix(errno);
			}
			block_size = lp_block_size(snum);
			if (bsize < block_size) {
				uint64_t factor = block_size/bsize;
				bsize = block_size;
				dsize /= factor;
				dfree /= factor;
			}
			if (bsize > block_size) {
				uint64_t factor = bsize/block_size;
				bsize = block_size;
				dsize *= factor;
				dfree *= factor;
			}
			sectors_per_unit = bsize/bytes_per_sector;
			DEBUG(5,("smbd_do_qfsinfo : SMB_QUERY_FS_SIZE_INFO bsize=%u, cSectorUnit=%u, \
cBytesSector=%u, cUnitTotal=%u, cUnitAvail=%d\n", (unsigned int)bsize, (unsigned int)sectors_per_unit,
				(unsigned int)bytes_per_sector, (unsigned int)dsize, (unsigned int)dfree));
			SBIG_UINT(pdata,0,dsize);
			SBIG_UINT(pdata,8,dfree);
			SIVAL(pdata,16,sectors_per_unit);
			SIVAL(pdata,20,bytes_per_sector);
			*fixed_portion = 24;
			break;
		}

		case SMB_FS_FULL_SIZE_INFORMATION:
		{
			uint64_t dfree,dsize,bsize,block_size,sectors_per_unit;
			data_len = 32;
			df_ret = get_dfree_info(conn, &smb_fname, &bsize,
						&dfree, &dsize);
			if (df_ret == (uint64_t)-1) {
				return map_nt_error_from_unix(errno);
			}
			block_size = lp_block_size(snum);
			if (bsize < block_size) {
				uint64_t factor = block_size/bsize;
				bsize = block_size;
				dsize /= factor;
				dfree /= factor;
			}
			if (bsize > block_size) {
				uint64_t factor = bsize/block_size;
				bsize = block_size;
				dsize *= factor;
				dfree *= factor;
			}
			sectors_per_unit = bsize/bytes_per_sector;
			DEBUG(5,("smbd_do_qfsinfo : SMB_QUERY_FS_FULL_SIZE_INFO bsize=%u, cSectorUnit=%u, \
cBytesSector=%u, cUnitTotal=%u, cUnitAvail=%d\n", (unsigned int)bsize, (unsigned int)sectors_per_unit,
				(unsigned int)bytes_per_sector, (unsigned int)dsize, (unsigned int)dfree));
			SBIG_UINT(pdata,0,dsize); /* Total Allocation units. */
			SBIG_UINT(pdata,8,dfree); /* Caller available allocation units. */
			SBIG_UINT(pdata,16,dfree); /* Actual available allocation units. */
			SIVAL(pdata,24,sectors_per_unit); /* Sectors per allocation unit. */
			SIVAL(pdata,28,bytes_per_sector); /* Bytes per sector. */
			*fixed_portion = 32;
			break;
		}

		case SMB_QUERY_FS_DEVICE_INFO:
		case SMB_FS_DEVICE_INFORMATION:
		{
			uint32_t characteristics = FILE_DEVICE_IS_MOUNTED;

			if (!CAN_WRITE(conn)) {
				characteristics |= FILE_READ_ONLY_DEVICE;
			}
			data_len = 8;
			SIVAL(pdata,0,FILE_DEVICE_DISK); /* dev type */
			SIVAL(pdata,4,characteristics);
			*fixed_portion = 8;
			break;
		}

#ifdef HAVE_SYS_QUOTAS
		case SMB_FS_QUOTA_INFORMATION:
		/* 
		 * what we have to send --metze:
		 *
		 * Unknown1: 		24 NULL bytes
		 * Soft Quota Treshold: 8 bytes seems like uint64_t or so
		 * Hard Quota Limit:	8 bytes seems like uint64_t or so
		 * Quota Flags:		2 byte :
		 * Unknown3:		6 NULL bytes
		 *
		 * 48 bytes total
		 * 
		 * details for Quota Flags:
		 * 
		 * 0x0020 Log Limit: log if the user exceeds his Hard Quota
		 * 0x0010 Log Warn:  log if the user exceeds his Soft Quota
		 * 0x0002 Deny Disk: deny disk access when the user exceeds his Hard Quota
		 * 0x0001 Enable Quotas: enable quota for this fs
		 *
		 */
		{
			/* we need to fake up a fsp here,
			 * because its not send in this call
			 */
			files_struct fsp;
			SMB_NTQUOTA_STRUCT quotas;

			ZERO_STRUCT(fsp);
			ZERO_STRUCT(quotas);

			fsp.conn = conn;
			fsp.fnum = FNUM_FIELD_INVALID;

			/* access check */
			if (get_current_uid(conn) != 0) {
				DEBUG(0,("get_user_quota: access_denied "
					 "service [%s] user [%s]\n",
					 lp_servicename(talloc_tos(), lp_sub, SNUM(conn)),
					 conn->session_info->unix_info->unix_name));
				return NT_STATUS_ACCESS_DENIED;
			}

			status = vfs_get_ntquota(&fsp, SMB_USER_FS_QUOTA_TYPE,
						 NULL, &quotas);
			if (!NT_STATUS_IS_OK(status)) {
				DEBUG(0,("vfs_get_ntquota() failed for service [%s]\n",lp_servicename(talloc_tos(), lp_sub, SNUM(conn))));
				return status;
			}

			data_len = 48;

			DEBUG(10,("SMB_FS_QUOTA_INFORMATION: for service [%s]\n",
				  lp_servicename(talloc_tos(), lp_sub, SNUM(conn))));

			/* Unknown1 24 NULL bytes*/
			SBIG_UINT(pdata,0,(uint64_t)0);
			SBIG_UINT(pdata,8,(uint64_t)0);
			SBIG_UINT(pdata,16,(uint64_t)0);

			/* Default Soft Quota 8 bytes */
			SBIG_UINT(pdata,24,quotas.softlim);

			/* Default Hard Quota 8 bytes */
			SBIG_UINT(pdata,32,quotas.hardlim);

			/* Quota flag 2 bytes */
			SSVAL(pdata,40,quotas.qflags);

			/* Unknown3 6 NULL bytes */
			SSVAL(pdata,42,0);
			SIVAL(pdata,44,0);

			break;
		}
#endif /* HAVE_SYS_QUOTAS */
		case SMB_FS_OBJECTID_INFORMATION:
		{
			unsigned char objid[16];
			struct smb_extended_info extended_info;
			memcpy(pdata,create_volume_objectid(conn, objid),16);
			samba_extended_info_version (&extended_info);
			SIVAL(pdata,16,extended_info.samba_magic);
			SIVAL(pdata,20,extended_info.samba_version);
			SIVAL(pdata,24,extended_info.samba_subversion);
			SBIG_UINT(pdata,28,extended_info.samba_gitcommitdate);
			memcpy(pdata+36,extended_info.samba_version_string,28);
			data_len = 64;
			break;
		}

		case SMB_FS_SECTOR_SIZE_INFORMATION:
		{
			data_len = 28;
			/*
			 * These values match a physical Windows Server 2012
			 * share backed by NTFS atop spinning rust.
			 */
			DEBUG(5, ("SMB_FS_SECTOR_SIZE_INFORMATION:"));
			/* logical_bytes_per_sector */
			SIVAL(pdata, 0, bytes_per_sector);
			/* phys_bytes_per_sector_atomic */
			SIVAL(pdata, 4, bytes_per_sector);
			/* phys_bytes_per_sector_perf */
			SIVAL(pdata, 8, bytes_per_sector);
			/* fs_effective_phys_bytes_per_sector_atomic */
			SIVAL(pdata, 12, bytes_per_sector);
			/* flags */
			SIVAL(pdata, 16, SSINFO_FLAGS_ALIGNED_DEVICE
				| SSINFO_FLAGS_PARTITION_ALIGNED_ON_DEVICE);
			/* byte_off_sector_align */
			SIVAL(pdata, 20, 0);
			/* byte_off_partition_align */
			SIVAL(pdata, 24, 0);
			*fixed_portion = 28;
			break;
		}


		/*
		 * Query the version and capabilities of the CIFS UNIX extensions
		 * in use.
		 */

		case SMB_QUERY_CIFS_UNIX_INFO:
		{
			bool large_write = lp_min_receive_file_size() &&
					!srv_is_signing_active(xconn);
			bool large_read = !srv_is_signing_active(xconn);
			int encrypt_caps = 0;

			if (!lp_unix_extensions()) {
				return NT_STATUS_INVALID_LEVEL;
			}

			switch (conn->encrypt_level) {
			case SMB_SIGNING_OFF:
				encrypt_caps = 0;
				break;
			case SMB_SIGNING_DESIRED:
			case SMB_SIGNING_IF_REQUIRED:
			case SMB_SIGNING_DEFAULT:
				encrypt_caps = CIFS_UNIX_TRANSPORT_ENCRYPTION_CAP;
				break;
			case SMB_SIGNING_REQUIRED:
				encrypt_caps = CIFS_UNIX_TRANSPORT_ENCRYPTION_CAP|
						CIFS_UNIX_TRANSPORT_ENCRYPTION_MANDATORY_CAP;
				large_write = false;
				large_read = false;
				break;
			}

			data_len = 12;
			SSVAL(pdata,0,CIFS_UNIX_MAJOR_VERSION);
			SSVAL(pdata,2,CIFS_UNIX_MINOR_VERSION);

			/* We have POSIX ACLs, pathname, encryption, 
			 * large read/write, and locking capability. */

			SBIG_UINT(pdata,4,((uint64_t)(
					CIFS_UNIX_POSIX_ACLS_CAP|
					CIFS_UNIX_POSIX_PATHNAMES_CAP|
					CIFS_UNIX_FCNTL_LOCKS_CAP|
					CIFS_UNIX_EXTATTR_CAP|
					CIFS_UNIX_POSIX_PATH_OPERATIONS_CAP|
					encrypt_caps|
					(large_read ? CIFS_UNIX_LARGE_READ_CAP : 0) |
					(large_write ?
					CIFS_UNIX_LARGE_WRITE_CAP : 0))));
			break;
		}

		case SMB_QUERY_POSIX_FS_INFO:
		{
			int rc;
			vfs_statvfs_struct svfs;

			if (!lp_unix_extensions()) {
				return NT_STATUS_INVALID_LEVEL;
			}

			rc = SMB_VFS_STATVFS(conn, &smb_fname, &svfs);

			if (!rc) {
				data_len = 56;
				SIVAL(pdata,0,svfs.OptimalTransferSize);
				SIVAL(pdata,4,svfs.BlockSize);
				SBIG_UINT(pdata,8,svfs.TotalBlocks);
				SBIG_UINT(pdata,16,svfs.BlocksAvail);
				SBIG_UINT(pdata,24,svfs.UserBlocksAvail);
				SBIG_UINT(pdata,32,svfs.TotalFileNodes);
				SBIG_UINT(pdata,40,svfs.FreeFileNodes);
				SBIG_UINT(pdata,48,svfs.FsIdentifier);
				DEBUG(5,("smbd_do_qfsinfo : SMB_QUERY_POSIX_FS_INFO succsessful\n"));
#ifdef EOPNOTSUPP
			} else if (rc == EOPNOTSUPP) {
				return NT_STATUS_INVALID_LEVEL;
#endif /* EOPNOTSUPP */
			} else {
				DEBUG(0,("vfs_statvfs() failed for service [%s]\n",lp_servicename(talloc_tos(), lp_sub, SNUM(conn))));
				return NT_STATUS_DOS(ERRSRV, ERRerror);
			}
			break;
		}

		case SMB_QUERY_POSIX_WHOAMI:
		{
			uint32_t flags = 0;
			uint32_t sid_bytes;
			int i;

			if (!lp_unix_extensions()) {
				return NT_STATUS_INVALID_LEVEL;
			}

			if (max_data_bytes < 40) {
				return NT_STATUS_BUFFER_TOO_SMALL;
			}

			if (security_session_user_level(conn->session_info, NULL) < SECURITY_USER) {
				flags |= SMB_WHOAMI_GUEST;
			}

			/* NOTE: 8 bytes for UID/GID, irrespective of native
			 * platform size. This matches
			 * SMB_QUERY_FILE_UNIX_BASIC and friends.
			 */
			data_len = 4 /* flags */
			    + 4 /* flag mask */
			    + 8 /* uid */
			    + 8 /* gid */
			    + 4 /* ngroups */
			    + 4 /* num_sids */
			    + 4 /* SID bytes */
			    + 4 /* pad/reserved */
			    + (conn->session_info->unix_token->ngroups * 8)
				/* groups list */
			    + (conn->session_info->security_token->num_sids *
				    SID_MAX_SIZE)
				/* SID list */;

			SIVAL(pdata, 0, flags);
			SIVAL(pdata, 4, SMB_WHOAMI_MASK);
			SBIG_UINT(pdata, 8,
				  (uint64_t)conn->session_info->unix_token->uid);
			SBIG_UINT(pdata, 16,
				  (uint64_t)conn->session_info->unix_token->gid);


			if (data_len >= max_data_bytes) {
				/* Potential overflow, skip the GIDs and SIDs. */

				SIVAL(pdata, 24, 0); /* num_groups */
				SIVAL(pdata, 28, 0); /* num_sids */
				SIVAL(pdata, 32, 0); /* num_sid_bytes */
				SIVAL(pdata, 36, 0); /* reserved */

				data_len = 40;
				break;
			}

			SIVAL(pdata, 24, conn->session_info->unix_token->ngroups);
			SIVAL(pdata, 28, conn->session_info->security_token->num_sids);

			/* We walk the SID list twice, but this call is fairly
			 * infrequent, and I don't expect that it's performance
			 * sensitive -- jpeach
			 */
			for (i = 0, sid_bytes = 0;
			     i < conn->session_info->security_token->num_sids; ++i) {
				sid_bytes += ndr_size_dom_sid(
					&conn->session_info->security_token->sids[i],
					0);
			}

			/* SID list byte count */
			SIVAL(pdata, 32, sid_bytes);

			/* 4 bytes pad/reserved - must be zero */
			SIVAL(pdata, 36, 0);
			data_len = 40;

			/* GID list */
			for (i = 0; i < conn->session_info->unix_token->ngroups; ++i) {
				SBIG_UINT(pdata, data_len,
					  (uint64_t)conn->session_info->unix_token->groups[i]);
				data_len += 8;
			}

			/* SID list */
			for (i = 0;
			    i < conn->session_info->security_token->num_sids; ++i) {
				int sid_len = ndr_size_dom_sid(
					&conn->session_info->security_token->sids[i],
					0);

				sid_linearize((uint8_t *)(pdata + data_len),
					      sid_len,
				    &conn->session_info->security_token->sids[i]);
				data_len += sid_len;
			}

			break;
		}

		case SMB_MAC_QUERY_FS_INFO:
			/*
			 * Thursby MAC extension... ONLY on NTFS filesystems
			 * once we do streams then we don't need this
			 */
			if (strequal(lp_fstype(SNUM(conn)),"NTFS")) {
				data_len = 88;
				SIVAL(pdata,84,0x100); /* Don't support mac... */
				break;
			}

			FALL_THROUGH;
		default:
			return NT_STATUS_INVALID_LEVEL;
	}

	*ret_data_len = data_len;
	return status;
}