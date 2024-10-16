static sector_t jfs_bmap(struct address_space *mapping, sector_t block)
{
	return generic_block_bmap(mapping, block, jfs_get_block);
}