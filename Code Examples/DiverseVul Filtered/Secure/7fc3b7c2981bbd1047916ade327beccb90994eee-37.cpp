static void udf_adjust_time(struct udf_inode_info *iinfo, struct timespec64 time)
{
	if (iinfo->i_crtime.tv_sec > time.tv_sec ||
	    (iinfo->i_crtime.tv_sec == time.tv_sec &&
	     iinfo->i_crtime.tv_nsec > time.tv_nsec))
		iinfo->i_crtime = time;
}