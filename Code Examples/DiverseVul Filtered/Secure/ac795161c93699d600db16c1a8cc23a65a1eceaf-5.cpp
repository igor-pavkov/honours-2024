void nfs_access_set_mask(struct nfs_access_entry *entry, u32 access_result)
{
	entry->mask = access_result;
}