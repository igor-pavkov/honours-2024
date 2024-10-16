static void __set_data_blkaddr(struct dnode_of_data *dn)
{
	struct f2fs_node *rn = F2FS_NODE(dn->node_page);
	__le32 *addr_array;

	/* Get physical address of data block */
	addr_array = blkaddr_in_node(rn);
	addr_array[dn->ofs_in_node] = cpu_to_le32(dn->data_blkaddr);
}