void reset_cifs_unix_caps(int xid, struct cifsTconInfo *tcon,
			  struct super_block *sb, struct smb_vol *vol_info)
{
	/* if we are reconnecting then should we check to see if
	 * any requested capabilities changed locally e.g. via
	 * remount but we can not do much about it here
	 * if they have (even if we could detect it by the following)
	 * Perhaps we could add a backpointer to array of sb from tcon
	 * or if we change to make all sb to same share the same
	 * sb as NFS - then we only have one backpointer to sb.
	 * What if we wanted to mount the server share twice once with
	 * and once without posixacls or posix paths? */
	__u64 saved_cap = le64_to_cpu(tcon->fsUnixInfo.Capability);

	if (vol_info && vol_info->no_linux_ext) {
		tcon->fsUnixInfo.Capability = 0;
		tcon->unix_ext = 0; /* Unix Extensions disabled */
		cFYI(1, "Linux protocol extensions disabled");
		return;
	} else if (vol_info)
		tcon->unix_ext = 1; /* Unix Extensions supported */

	if (tcon->unix_ext == 0) {
		cFYI(1, "Unix extensions disabled so not set on reconnect");
		return;
	}

	if (!CIFSSMBQFSUnixInfo(xid, tcon)) {
		__u64 cap = le64_to_cpu(tcon->fsUnixInfo.Capability);

		/* check for reconnect case in which we do not
		   want to change the mount behavior if we can avoid it */
		if (vol_info == NULL) {
			/* turn off POSIX ACL and PATHNAMES if not set
			   originally at mount time */
			if ((saved_cap & CIFS_UNIX_POSIX_ACL_CAP) == 0)
				cap &= ~CIFS_UNIX_POSIX_ACL_CAP;
			if ((saved_cap & CIFS_UNIX_POSIX_PATHNAMES_CAP) == 0) {
				if (cap & CIFS_UNIX_POSIX_PATHNAMES_CAP)
					cERROR(1, "POSIXPATH support change");
				cap &= ~CIFS_UNIX_POSIX_PATHNAMES_CAP;
			} else if ((cap & CIFS_UNIX_POSIX_PATHNAMES_CAP) == 0) {
				cERROR(1, "possible reconnect error");
				cERROR(1, "server disabled POSIX path support");
			}
		}

		cap &= CIFS_UNIX_CAP_MASK;
		if (vol_info && vol_info->no_psx_acl)
			cap &= ~CIFS_UNIX_POSIX_ACL_CAP;
		else if (CIFS_UNIX_POSIX_ACL_CAP & cap) {
			cFYI(1, "negotiated posix acl support");
			if (sb)
				sb->s_flags |= MS_POSIXACL;
		}

		if (vol_info && vol_info->posix_paths == 0)
			cap &= ~CIFS_UNIX_POSIX_PATHNAMES_CAP;
		else if (cap & CIFS_UNIX_POSIX_PATHNAMES_CAP) {
			cFYI(1, "negotiate posix pathnames");
			if (sb)
				CIFS_SB(sb)->mnt_cifs_flags |=
					CIFS_MOUNT_POSIX_PATHS;
		}

		/* We might be setting the path sep back to a different
		form if we are reconnecting and the server switched its
		posix path capability for this share */
		if (sb && (CIFS_SB(sb)->prepathlen > 0))
			CIFS_SB(sb)->prepath[0] = CIFS_DIR_SEP(CIFS_SB(sb));

		if (sb && (CIFS_SB(sb)->rsize > 127 * 1024)) {
			if ((cap & CIFS_UNIX_LARGE_READ_CAP) == 0) {
				CIFS_SB(sb)->rsize = 127 * 1024;
				cFYI(DBG2, "larger reads not supported by srv");
			}
		}


		cFYI(1, "Negotiate caps 0x%x", (int)cap);
#ifdef CONFIG_CIFS_DEBUG2
		if (cap & CIFS_UNIX_FCNTL_CAP)
			cFYI(1, "FCNTL cap");
		if (cap & CIFS_UNIX_EXTATTR_CAP)
			cFYI(1, "EXTATTR cap");
		if (cap & CIFS_UNIX_POSIX_PATHNAMES_CAP)
			cFYI(1, "POSIX path cap");
		if (cap & CIFS_UNIX_XATTR_CAP)
			cFYI(1, "XATTR cap");
		if (cap & CIFS_UNIX_POSIX_ACL_CAP)
			cFYI(1, "POSIX ACL cap");
		if (cap & CIFS_UNIX_LARGE_READ_CAP)
			cFYI(1, "very large read cap");
		if (cap & CIFS_UNIX_LARGE_WRITE_CAP)
			cFYI(1, "very large write cap");
#endif /* CIFS_DEBUG2 */
		if (CIFSSMBSetFSUnixInfo(xid, tcon, cap)) {
			if (vol_info == NULL) {
				cFYI(1, "resetting capabilities failed");
			} else
				cERROR(1, "Negotiating Unix capabilities "
					   "with the server failed.  Consider "
					   "mounting with the Unix Extensions\n"
					   "disabled, if problems are found, "
					   "by specifying the nounix mount "
					   "option.");

		}
	}
}