static inline struct page *shmem_swapin(swp_entry_t swap, gfp_t gfp,
			struct shmem_inode_info *info, pgoff_t index)
{
	return swapin_readahead(swap, gfp, NULL, 0);
}