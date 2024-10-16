static inline struct resv_map *inode_resv_map(struct inode *inode)
{
	return inode->i_mapping->private_data;
}