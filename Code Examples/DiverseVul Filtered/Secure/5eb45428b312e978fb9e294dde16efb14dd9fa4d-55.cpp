static int _recursive_rmdir(char *dirname, dev_t pdev,
			    const char *exclude, int level, bool onedev)
{
	struct dirent *direntp;
	DIR *dir;
	int ret, failed=0;
	char pathname[MAXPATHLEN];
	bool hadexclude = false;

	dir = opendir(dirname);
	if (!dir) {
		ERROR("failed to open %s", dirname);
		return -1;
	}

	while ((direntp = readdir(dir))) {
		struct stat mystat;
		int rc;

		if (!direntp)
			break;

		if (!strcmp(direntp->d_name, ".") ||
		    !strcmp(direntp->d_name, ".."))
			continue;

		rc = snprintf(pathname, MAXPATHLEN, "%s/%s", dirname, direntp->d_name);
		if (rc < 0 || rc >= MAXPATHLEN) {
			ERROR("pathname too long");
			failed=1;
			continue;
		}

		if (!level && exclude && !strcmp(direntp->d_name, exclude)) {
			ret = rmdir(pathname);
			if (ret < 0) {
				switch(errno) {
				case ENOTEMPTY:
					INFO("Not deleting snapshot %s", pathname);
					hadexclude = true;
					break;
				case ENOTDIR:
					ret = unlink(pathname);
					if (ret)
						INFO("Failed to remove %s", pathname);
					break;
				default:
					SYSERROR("Failed to rmdir %s", pathname);
					failed = 1;
					break;
				}
			}
			continue;
		}

		ret = lstat(pathname, &mystat);
		if (ret) {
			ERROR("Failed to stat %s", pathname);
			failed = 1;
			continue;
		}
		if (onedev && mystat.st_dev != pdev) {
			/* TODO should we be checking /proc/self/mountinfo for
			 * pathname and not doing this if found? */
			if (btrfs_try_remove_subvol(pathname))
				INFO("Removed btrfs subvolume at %s\n", pathname);
			continue;
		}
		if (S_ISDIR(mystat.st_mode)) {
			if (_recursive_rmdir(pathname, pdev, exclude, level+1, onedev) < 0)
				failed=1;
		} else {
			if (unlink(pathname) < 0) {
				SYSERROR("Failed to delete %s", pathname);
				failed=1;
			}
		}
	}

	if (rmdir(dirname) < 0 && !btrfs_try_remove_subvol(dirname) && !hadexclude) {
		ERROR("Failed to delete %s", dirname);
		failed=1;
	}

	ret = closedir(dir);
	if (ret) {
		ERROR("Failed to close directory %s", dirname);
		failed=1;
	}

	return failed ? -1 : 0;
}