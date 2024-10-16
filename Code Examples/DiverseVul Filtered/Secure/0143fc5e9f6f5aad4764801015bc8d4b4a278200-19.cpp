static int udf_encode_fh(struct inode *inode, __u32 *fh, int *lenp,
			 struct inode *parent)
{
	int len = *lenp;
	struct kernel_lb_addr location = UDF_I(inode)->i_location;
	struct fid *fid = (struct fid *)fh;
	int type = FILEID_UDF_WITHOUT_PARENT;

	if (parent && (len < 5)) {
		*lenp = 5;
		return 255;
	} else if (len < 3) {
		*lenp = 3;
		return 255;
	}

	*lenp = 3;
	fid->udf.block = location.logicalBlockNum;
	fid->udf.partref = location.partitionReferenceNum;
	fid->udf.parent_partref = 0;
	fid->udf.generation = inode->i_generation;

	if (parent) {
		location = UDF_I(parent)->i_location;
		fid->udf.parent_block = location.logicalBlockNum;
		fid->udf.parent_partref = location.partitionReferenceNum;
		fid->udf.parent_generation = inode->i_generation;
		*lenp = 5;
		type = FILEID_UDF_WITH_PARENT;
	}

	return type;
}