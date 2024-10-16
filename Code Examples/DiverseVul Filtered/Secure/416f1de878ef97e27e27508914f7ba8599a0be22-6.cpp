int csync_setBackupFileStatus(char *filename, int backupDirLength)
{

	struct stat buf;
	int rc = stat((filename + backupDirLength), &buf);
	if (rc == 0) {
		csync_debug(1, "Stating original file %s rc: %d mode: %o \n", (filename + backupDirLength), rc, buf.st_mode);

		rc = chown(filename, buf.st_uid, buf.st_gid);
		csync_debug(rc == 0, "Changing owner of %s to user %d and group %d, rc= %d \n", filename, buf.st_uid, buf.st_gid, rc);

		rc = chmod(filename, buf.st_mode);
		csync_debug(rc == 0, "Changing mode of %s to mode %o, rc= %d \n", filename, buf.st_mode, rc);

	} else {
		csync_debug(0, "Error getting mode and owner ship from %s \n", (filename + backupDirLength));
		return -1;
	}
	return 0;
};