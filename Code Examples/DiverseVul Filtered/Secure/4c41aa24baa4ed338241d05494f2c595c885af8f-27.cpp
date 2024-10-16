static inline void ncp_extract_nfs_info(const unsigned char *structure,
				 struct nw_nfs_info *target)
{
	target->mode = DVAL_LH(structure);
	target->rdev = DVAL_LH(structure + 8);
}