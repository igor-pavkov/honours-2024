static int may_context_mount_sb_relabel(u32 sid,
			struct superblock_security_struct *sbsec,
			const struct cred *cred)
{
	const struct task_security_struct *tsec = selinux_cred(cred);
	int rc;

	rc = avc_has_perm(&selinux_state,
			  tsec->sid, sbsec->sid, SECCLASS_FILESYSTEM,
			  FILESYSTEM__RELABELFROM, NULL);
	if (rc)
		return rc;

	rc = avc_has_perm(&selinux_state,
			  tsec->sid, sid, SECCLASS_FILESYSTEM,
			  FILESYSTEM__RELABELTO, NULL);
	return rc;
}