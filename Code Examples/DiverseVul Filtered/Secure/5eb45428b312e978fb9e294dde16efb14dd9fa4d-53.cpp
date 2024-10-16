char *choose_init(const char *rootfs)
{
	char *retv = NULL;
	const char *empty = "",
		   *tmp;
	int ret, env_set = 0;

	if (!getenv("PATH")) {
		if (setenv("PATH", "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin", 0))
			SYSERROR("Failed to setenv");
		env_set = 1;
	}

	retv = on_path("init.lxc", rootfs);

	if (env_set) {
		if (unsetenv("PATH"))
			SYSERROR("Failed to unsetenv");
	}

	if (retv)
		return retv;

	retv = malloc(PATH_MAX);
	if (!retv)
		return NULL;

	if (rootfs)
		tmp = rootfs;
	else
		tmp = empty;

	ret = snprintf(retv, PATH_MAX, "%s/%s/%s", tmp, SBINDIR, "/init.lxc");
	if (ret < 0 || ret >= PATH_MAX) {
		ERROR("pathname too long");
		goto out1;
	}
	if (access(retv, X_OK) == 0)
		return retv;

	ret = snprintf(retv, PATH_MAX, "%s/%s/%s", tmp, LXCINITDIR, "/lxc/lxc-init");
	if (ret < 0 || ret >= PATH_MAX) {
		ERROR("pathname too long");
		goto out1;
	}
	if (access(retv, X_OK) == 0)
		return retv;

	ret = snprintf(retv, PATH_MAX, "%s/usr/lib/lxc/lxc-init", tmp);
	if (ret < 0 || ret >= PATH_MAX) {
		ERROR("pathname too long");
		goto out1;
	}
	if (access(retv, X_OK) == 0)
		return retv;

	ret = snprintf(retv, PATH_MAX, "%s/sbin/lxc-init", tmp);
	if (ret < 0 || ret >= PATH_MAX) {
		ERROR("pathname too long");
		goto out1;
	}
	if (access(retv, X_OK) == 0)
		return retv;

	/*
	 * Last resort, look for the statically compiled init.lxc which we
	 * hopefully bind-mounted in.
	 * If we are called during container setup, and we get to this point,
	 * then the init.lxc.static from the host will need to be bind-mounted
	 * in.  So we return NULL here to indicate that.
	 */
	if (rootfs)
		goto out1;

	ret = snprintf(retv, PATH_MAX, "/init.lxc.static");
	if (ret < 0 || ret >= PATH_MAX) {
		WARN("Nonsense - name /lxc.init.static too long");
		goto out1;
	}
	if (access(retv, X_OK) == 0)
		return retv;

out1:
	free(retv);
	return NULL;
}