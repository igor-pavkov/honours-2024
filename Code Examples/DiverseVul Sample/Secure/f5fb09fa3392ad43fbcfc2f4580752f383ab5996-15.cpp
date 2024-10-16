static sector_t minix_bmap(struct address_space *mapping, sector_t block)
{
	return generic_block_bmap(mapping,block,minix_get_block);
}