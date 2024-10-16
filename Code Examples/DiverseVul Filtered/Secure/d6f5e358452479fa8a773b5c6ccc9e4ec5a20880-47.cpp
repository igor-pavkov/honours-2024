static void smb2_decrypt_offload(struct work_struct *work)
{
	struct smb2_decrypt_work *dw = container_of(work,
				struct smb2_decrypt_work, decrypt);
	int i, rc;
	struct mid_q_entry *mid;

	rc = decrypt_raw_data(dw->server, dw->buf, dw->server->vals->read_rsp_size,
			      dw->ppages, dw->npages, dw->len, true);
	if (rc) {
		cifs_dbg(VFS, "error decrypting rc=%d\n", rc);
		goto free_pages;
	}

	dw->server->lstrp = jiffies;
	mid = smb2_find_dequeue_mid(dw->server, dw->buf);
	if (mid == NULL)
		cifs_dbg(FYI, "mid not found\n");
	else {
		mid->decrypted = true;
		rc = handle_read_data(dw->server, mid, dw->buf,
				      dw->server->vals->read_rsp_size,
				      dw->ppages, dw->npages, dw->len,
				      true);
		if (rc >= 0) {
#ifdef CONFIG_CIFS_STATS2
			mid->when_received = jiffies;
#endif
			if (dw->server->ops->is_network_name_deleted)
				dw->server->ops->is_network_name_deleted(dw->buf,
									 dw->server);

			mid->callback(mid);
		} else {
			spin_lock(&cifs_tcp_ses_lock);
			spin_lock(&GlobalMid_Lock);
			if (dw->server->tcpStatus == CifsNeedReconnect) {
				mid->mid_state = MID_RETRY_NEEDED;
				spin_unlock(&GlobalMid_Lock);
				spin_unlock(&cifs_tcp_ses_lock);
				mid->callback(mid);
			} else {
				mid->mid_state = MID_REQUEST_SUBMITTED;
				mid->mid_flags &= ~(MID_DELETED);
				list_add_tail(&mid->qhead,
					&dw->server->pending_mid_q);
				spin_unlock(&GlobalMid_Lock);
				spin_unlock(&cifs_tcp_ses_lock);
			}
		}
		cifs_mid_q_entry_release(mid);
	}

free_pages:
	for (i = dw->npages-1; i >= 0; i--)
		put_page(dw->ppages[i]);

	kfree(dw->ppages);
	cifs_small_buf_release(dw->buf);
	kfree(dw);
}