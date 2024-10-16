static int nfs_readdir_page_filler(struct nfs_readdir_descriptor *desc,
				   struct nfs_entry *entry,
				   struct page **xdr_pages,
				   unsigned int buflen,
				   struct page **arrays,
				   size_t narrays)
{
	struct address_space *mapping = desc->file->f_mapping;
	struct xdr_stream stream;
	struct xdr_buf buf;
	struct page *scratch, *new, *page = *arrays;
	int status;

	scratch = alloc_page(GFP_KERNEL);
	if (scratch == NULL)
		return -ENOMEM;

	xdr_init_decode_pages(&stream, &buf, xdr_pages, buflen);
	xdr_set_scratch_page(&stream, scratch);

	do {
		if (entry->fattr->label)
			entry->fattr->label->len = NFS4_MAXLABELLEN;

		status = xdr_decode(desc, entry, &stream);
		if (status != 0)
			break;

		if (desc->plus)
			nfs_prime_dcache(file_dentry(desc->file), entry,
					desc->dir_verifier);

		status = nfs_readdir_add_to_array(entry, page);
		if (status != -ENOSPC)
			continue;

		if (page->mapping != mapping) {
			if (!--narrays)
				break;
			new = nfs_readdir_page_array_alloc(entry->prev_cookie,
							   GFP_KERNEL);
			if (!new)
				break;
			arrays++;
			*arrays = page = new;
		} else {
			new = nfs_readdir_page_get_next(mapping,
							page->index + 1,
							entry->prev_cookie);
			if (!new)
				break;
			if (page != *arrays)
				nfs_readdir_page_unlock_and_put(page);
			page = new;
		}
		status = nfs_readdir_add_to_array(entry, page);
	} while (!status && !entry->eof);

	switch (status) {
	case -EBADCOOKIE:
		if (entry->eof) {
			nfs_readdir_page_set_eof(page);
			status = 0;
		}
		break;
	case -ENOSPC:
	case -EAGAIN:
		status = 0;
		break;
	}

	if (page != *arrays)
		nfs_readdir_page_unlock_and_put(page);

	put_page(scratch);
	return status;
}