int open_cached_dir(unsigned int xid, struct cifs_tcon *tcon,
		const char *path,
		struct cifs_sb_info *cifs_sb,
		struct cached_fid **cfid)
{
	struct cifs_ses *ses = tcon->ses;
	struct TCP_Server_Info *server = ses->server;
	struct cifs_open_parms oparms;
	struct smb2_create_rsp *o_rsp = NULL;
	struct smb2_query_info_rsp *qi_rsp = NULL;
	int resp_buftype[2];
	struct smb_rqst rqst[2];
	struct kvec rsp_iov[2];
	struct kvec open_iov[SMB2_CREATE_IOV_SIZE];
	struct kvec qi_iov[1];
	int rc, flags = 0;
	__le16 utf16_path = 0; /* Null - since an open of top of share */
	u8 oplock = SMB2_OPLOCK_LEVEL_II;
	struct cifs_fid *pfid;
	struct dentry *dentry;

	if (tcon->nohandlecache)
		return -ENOTSUPP;

	if (cifs_sb->root == NULL)
		return -ENOENT;

	if (strlen(path))
		return -ENOENT;

	dentry = cifs_sb->root;

	mutex_lock(&tcon->crfid.fid_mutex);
	if (tcon->crfid.is_valid) {
		cifs_dbg(FYI, "found a cached root file handle\n");
		*cfid = &tcon->crfid;
		kref_get(&tcon->crfid.refcount);
		mutex_unlock(&tcon->crfid.fid_mutex);
		return 0;
	}

	/*
	 * We do not hold the lock for the open because in case
	 * SMB2_open needs to reconnect, it will end up calling
	 * cifs_mark_open_files_invalid() which takes the lock again
	 * thus causing a deadlock
	 */

	mutex_unlock(&tcon->crfid.fid_mutex);

	if (smb3_encryption_required(tcon))
		flags |= CIFS_TRANSFORM_REQ;

	if (!server->ops->new_lease_key)
		return -EIO;

	pfid = tcon->crfid.fid;
	server->ops->new_lease_key(pfid);

	memset(rqst, 0, sizeof(rqst));
	resp_buftype[0] = resp_buftype[1] = CIFS_NO_BUFFER;
	memset(rsp_iov, 0, sizeof(rsp_iov));

	/* Open */
	memset(&open_iov, 0, sizeof(open_iov));
	rqst[0].rq_iov = open_iov;
	rqst[0].rq_nvec = SMB2_CREATE_IOV_SIZE;

	oparms.tcon = tcon;
	oparms.create_options = cifs_create_options(cifs_sb, 0);
	oparms.desired_access = FILE_READ_ATTRIBUTES;
	oparms.disposition = FILE_OPEN;
	oparms.fid = pfid;
	oparms.reconnect = false;

	rc = SMB2_open_init(tcon, server,
			    &rqst[0], &oplock, &oparms, &utf16_path);
	if (rc)
		goto oshr_free;
	smb2_set_next_command(tcon, &rqst[0]);

	memset(&qi_iov, 0, sizeof(qi_iov));
	rqst[1].rq_iov = qi_iov;
	rqst[1].rq_nvec = 1;

	rc = SMB2_query_info_init(tcon, server,
				  &rqst[1], COMPOUND_FID,
				  COMPOUND_FID, FILE_ALL_INFORMATION,
				  SMB2_O_INFO_FILE, 0,
				  sizeof(struct smb2_file_all_info) +
				  PATH_MAX * 2, 0, NULL);
	if (rc)
		goto oshr_free;

	smb2_set_related(&rqst[1]);

	rc = compound_send_recv(xid, ses, server,
				flags, 2, rqst,
				resp_buftype, rsp_iov);
	mutex_lock(&tcon->crfid.fid_mutex);

	/*
	 * Now we need to check again as the cached root might have
	 * been successfully re-opened from a concurrent process
	 */

	if (tcon->crfid.is_valid) {
		/* work was already done */

		/* stash fids for close() later */
		struct cifs_fid fid = {
			.persistent_fid = pfid->persistent_fid,
			.volatile_fid = pfid->volatile_fid,
		};

		/*
		 * caller expects this func to set the fid in crfid to valid
		 * cached root, so increment the refcount.
		 */
		kref_get(&tcon->crfid.refcount);

		mutex_unlock(&tcon->crfid.fid_mutex);

		if (rc == 0) {
			/* close extra handle outside of crit sec */
			SMB2_close(xid, tcon, fid.persistent_fid, fid.volatile_fid);
		}
		rc = 0;
		goto oshr_free;
	}

	/* Cached root is still invalid, continue normaly */

	if (rc) {
		if (rc == -EREMCHG) {
			tcon->need_reconnect = true;
			pr_warn_once("server share %s deleted\n",
				     tcon->treeName);
		}
		goto oshr_exit;
	}

	atomic_inc(&tcon->num_remote_opens);

	o_rsp = (struct smb2_create_rsp *)rsp_iov[0].iov_base;
	oparms.fid->persistent_fid = o_rsp->PersistentFileId;
	oparms.fid->volatile_fid = o_rsp->VolatileFileId;
#ifdef CONFIG_CIFS_DEBUG2
	oparms.fid->mid = le64_to_cpu(o_rsp->hdr.MessageId);
#endif /* CIFS_DEBUG2 */

	tcon->crfid.tcon = tcon;
	tcon->crfid.is_valid = true;
	tcon->crfid.dentry = dentry;
	dget(dentry);
	kref_init(&tcon->crfid.refcount);

	/* BB TBD check to see if oplock level check can be removed below */
	if (o_rsp->OplockLevel == SMB2_OPLOCK_LEVEL_LEASE) {
		/*
		 * See commit 2f94a3125b87. Increment the refcount when we
		 * get a lease for root, release it if lease break occurs
		 */
		kref_get(&tcon->crfid.refcount);
		tcon->crfid.has_lease = true;
		smb2_parse_contexts(server, o_rsp,
				&oparms.fid->epoch,
				    oparms.fid->lease_key, &oplock,
				    NULL, NULL);
	} else
		goto oshr_exit;

	qi_rsp = (struct smb2_query_info_rsp *)rsp_iov[1].iov_base;
	if (le32_to_cpu(qi_rsp->OutputBufferLength) < sizeof(struct smb2_file_all_info))
		goto oshr_exit;
	if (!smb2_validate_and_copy_iov(
				le16_to_cpu(qi_rsp->OutputBufferOffset),
				sizeof(struct smb2_file_all_info),
				&rsp_iov[1], sizeof(struct smb2_file_all_info),
				(char *)&tcon->crfid.file_all_info))
		tcon->crfid.file_all_info_is_valid = true;
	tcon->crfid.time = jiffies;


oshr_exit:
	mutex_unlock(&tcon->crfid.fid_mutex);
oshr_free:
	SMB2_open_free(&rqst[0]);
	SMB2_query_info_free(&rqst[1]);
	free_rsp_buf(resp_buftype[0], rsp_iov[0].iov_base);
	free_rsp_buf(resp_buftype[1], rsp_iov[1].iov_base);
	if (rc == 0)
		*cfid = &tcon->crfid;
	return rc;
}