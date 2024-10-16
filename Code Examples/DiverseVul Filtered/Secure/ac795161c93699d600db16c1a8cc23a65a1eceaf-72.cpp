int nfs_readdir_add_to_array(struct nfs_entry *entry, struct page *page)
{
	struct nfs_cache_array *array;
	struct nfs_cache_array_entry *cache_entry;
	const char *name;
	int ret;

	name = nfs_readdir_copy_name(entry->name, entry->len);
	if (!name)
		return -ENOMEM;

	array = kmap_atomic(page);
	ret = nfs_readdir_array_can_expand(array);
	if (ret) {
		kfree(name);
		goto out;
	}

	cache_entry = &array->array[array->size];
	cache_entry->cookie = entry->prev_cookie;
	cache_entry->ino = entry->ino;
	cache_entry->d_type = entry->d_type;
	cache_entry->name_len = entry->len;
	cache_entry->name = name;
	array->last_cookie = entry->cookie;
	if (array->last_cookie <= cache_entry->cookie)
		array->cookies_are_ordered = 0;
	array->size++;
	if (entry->eof != 0)
		nfs_readdir_array_set_eof(array);
out:
	kunmap_atomic(array);
	return ret;
}