static int udf_alloc_i_data(struct inode *inode, size_t size)
{
	struct udf_inode_info *iinfo = UDF_I(inode);
	iinfo->i_data = kmalloc(size, GFP_KERNEL);
	if (!iinfo->i_data)
		return -ENOMEM;
	return 0;
}