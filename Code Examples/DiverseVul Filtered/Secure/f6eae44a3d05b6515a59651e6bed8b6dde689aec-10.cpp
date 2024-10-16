static int parse_opt_token(const char *token)
{
	if (token == NULL)
		return OPT_ERROR;

	if (strncmp(token, "users", 5) == 0)
		return OPT_USERS;
	if (strncmp(token, "user_xattr", 10) == 0)
		return OPT_USER_XATTR;
	if (strncmp(token, "user", 4) == 0)
		return OPT_USER;
	if (strncmp(token, "pass", 4) == 0)
		return OPT_PASS;
	if (strncmp(token, "sec", 3) == 0)
		return OPT_SEC;
	if (strncmp(token, "ip", 2) == 0)
		return OPT_IP;
	if (strncmp(token, "unc", 3) == 0 ||
		strncmp(token, "target", 6) == 0 ||
		strncmp(token, "path", 4) == 0)
		return OPT_UNC;
	if (strncmp(token, "dom", 3) == 0 || strncmp(token, "workg", 5) == 0)
		return OPT_DOM;
	if (strncmp(token, "cred", 4) == 0)
		return OPT_CRED;
	if (strncmp(token, "uid", 3) == 0)
		return OPT_UID;
	if (strncmp(token, "cruid", 5) == 0)
		return OPT_CRUID;
	if (strncmp(token, "gid", 3) == 0)
		return OPT_GID;
	if (strncmp(token, "fmask", 5) == 0)
		return OPT_FMASK;
	if (strncmp(token, "file_mode", 9) == 0)
		return OPT_FILE_MODE;
	if (strncmp(token, "dmask", 5) == 0)
		return OPT_DMASK;
	if (strncmp(token, "dir_mode", 4) == 0 || strncmp(token, "dirm", 4) == 0)
		return OPT_DIR_MODE;
	if (strncmp(token, "nosuid", 6) == 0)
		return OPT_NO_SUID;
	if (strncmp(token, "suid", 4) == 0)
		return OPT_SUID;
	if (strncmp(token, "nodev", 5) == 0)
		return OPT_NO_DEV;
	if (strncmp(token, "nobrl", 5) == 0 || strncmp(token, "nolock", 6) == 0)
		return OPT_NO_LOCK;
	if (strncmp(token, "mand", 4) == 0)
		return OPT_MAND;
	if (strncmp(token, "nomand", 6) == 0)
		return OPT_NOMAND;
	if (strncmp(token, "dev", 3) == 0)
		return OPT_DEV;
	if (strncmp(token, "noexec", 6) == 0)
		return OPT_NO_EXEC;
	if (strncmp(token, "exec", 4) == 0)
		return OPT_EXEC;
	if (strncmp(token, "guest", 5) == 0)
		return OPT_GUEST;
	if (strncmp(token, "ro", 2) == 0)
		return OPT_RO;
	if (strncmp(token, "rw", 2) == 0)
		return OPT_RW;
	if (strncmp(token, "remount", 7) == 0)
		return OPT_REMOUNT;
	if (strncmp(token, "_netdev", 7) == 0)
		return OPT_IGNORE;

	return OPT_ERROR;
}