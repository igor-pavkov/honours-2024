static struct dentry *udf_fh_to_dentry(struct super_block *sb,
				       struct fid *fid, int fh_len, int fh_type)
{
	if ((fh_len != 3 && fh_len != 5) ||
	    (fh_type != FILEID_UDF_WITH_PARENT &&
	     fh_type != FILEID_UDF_WITHOUT_PARENT))
		return NULL;

	return udf_nfs_get_inode(sb, fid->udf.block, fid->udf.partref,
			fid->udf.generation);
}