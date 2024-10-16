static fmode_t flags_to_mode(int flags)
{
	fmode_t res = (__force fmode_t)flags & FMODE_EXEC;
	if ((flags & O_ACCMODE) != O_WRONLY)
		res |= FMODE_READ;
	if ((flags & O_ACCMODE) != O_RDONLY)
		res |= FMODE_WRITE;
	return res;
}