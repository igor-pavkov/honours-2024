static int nfs_open_permission_mask(int openflags)
{
	int mask = 0;

	if (openflags & __FMODE_EXEC) {
		/* ONLY check exec rights */
		mask = MAY_EXEC;
	} else {
		if ((openflags & O_ACCMODE) != O_WRONLY)
			mask |= MAY_READ;
		if ((openflags & O_ACCMODE) != O_RDONLY)
			mask |= MAY_WRITE;
	}

	return mask;
}