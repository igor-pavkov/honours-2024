static void samba_extended_info_version(struct smb_extended_info *extended_info)
{
	SMB_ASSERT(extended_info != NULL);

	extended_info->samba_magic = SAMBA_EXTENDED_INFO_MAGIC;
	extended_info->samba_version = ((SAMBA_VERSION_MAJOR & 0xff) << 24)
				       | ((SAMBA_VERSION_MINOR & 0xff) << 16)
				       | ((SAMBA_VERSION_RELEASE & 0xff) << 8);
#ifdef SAMBA_VERSION_REVISION
	extended_info->samba_version |= (tolower(*SAMBA_VERSION_REVISION) - 'a' + 1) & 0xff;
#endif
	extended_info->samba_subversion = 0;
#ifdef SAMBA_VERSION_RC_RELEASE
	extended_info->samba_subversion |= (SAMBA_VERSION_RC_RELEASE & 0xff) << 24;
#else
#ifdef SAMBA_VERSION_PRE_RELEASE
	extended_info->samba_subversion |= (SAMBA_VERSION_PRE_RELEASE & 0xff) << 16;
#endif
#endif
#ifdef SAMBA_VERSION_VENDOR_PATCH
	extended_info->samba_subversion |= (SAMBA_VERSION_VENDOR_PATCH & 0xffff);
#endif
	extended_info->samba_gitcommitdate = 0;
#ifdef SAMBA_VERSION_COMMIT_TIME
	unix_to_nt_time(&extended_info->samba_gitcommitdate, SAMBA_VERSION_COMMIT_TIME);
#endif

	memset(extended_info->samba_version_string, 0,
	       sizeof(extended_info->samba_version_string));

	snprintf (extended_info->samba_version_string,
		  sizeof(extended_info->samba_version_string),
		  "%s", samba_version_string());
}