void init_stat_ex_from_stat (struct stat_ex *dst,
			    const struct stat *src,
			    bool fake_dir_create_times)
{
	dst->st_ex_dev = src->st_dev;
	dst->st_ex_ino = src->st_ino;
	dst->st_ex_mode = src->st_mode;
	dst->st_ex_nlink = src->st_nlink;
	dst->st_ex_uid = src->st_uid;
	dst->st_ex_gid = src->st_gid;
	dst->st_ex_rdev = src->st_rdev;
	dst->st_ex_size = src->st_size;
	dst->st_ex_atime = get_atimespec(src);
	dst->st_ex_mtime = get_mtimespec(src);
	dst->st_ex_ctime = get_ctimespec(src);
	make_create_timespec(src, dst, fake_dir_create_times);
#ifdef HAVE_STAT_ST_BLKSIZE
	dst->st_ex_blksize = src->st_blksize;
#else
	dst->st_ex_blksize = STAT_ST_BLOCKSIZE;
#endif

#ifdef HAVE_STAT_ST_BLOCKS
	dst->st_ex_blocks = src->st_blocks;
#else
	dst->st_ex_blocks = src->st_size / dst->st_ex_blksize + 1;
#endif

#ifdef HAVE_STAT_ST_FLAGS
	dst->st_ex_flags = src->st_flags;
#else
	dst->st_ex_flags = 0;
#endif
}