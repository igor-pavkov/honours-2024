R_API RList *r_bin_file_compute_hashes(RBin *bin, ut64 limit) {
	r_return_val_if_fail (bin && bin->cur && bin->cur->o, NULL);
	ut64 buf_len = 0, r = 0;
	RBinFile *bf = bin->cur;
	RBinObject *o = bf->o;

	RIODesc *iod = r_io_desc_get (bin->iob.io, bf->fd);
	if (!iod) {
		return NULL;
	}

	buf_len = r_io_desc_size (iod);
	// By SLURP_LIMIT normally cannot compute ...
	if (buf_len > limit) {
		if (bin->verbose) {
			eprintf ("Warning: r_bin_file_hash: file exceeds bin.hashlimit\n");
		}
		return NULL;
	}
	const size_t blocksize = 64000;
	ut8 *buf = malloc (blocksize);
	if (!buf) {
		eprintf ("Cannot allocate computation buffer\n");
		return NULL;
	}

	char hash[128];
	RHash *ctx = r_hash_new (false, R_HASH_MD5 | R_HASH_SHA1 | R_HASH_SHA256);
	while (r + blocksize < buf_len) {
		r_io_desc_seek (iod, r, R_IO_SEEK_SET);
		int b = r_io_desc_read (iod, buf, blocksize);
		(void)r_hash_do_md5 (ctx, buf, blocksize);
		(void)r_hash_do_sha1 (ctx, buf, blocksize);
		(void)r_hash_do_sha256 (ctx, buf, blocksize);
		r += b;
	}
	if (r < buf_len) {
		r_io_desc_seek (iod, r, R_IO_SEEK_SET);
		const size_t rem_len = buf_len-r;
		int b = r_io_desc_read (iod, buf, rem_len);
		if (b < 1) {
			eprintf ("r_io_desc_read: error\n");
		} else {
			(void)r_hash_do_md5 (ctx, buf, b);
			(void)r_hash_do_sha1 (ctx, buf, b);
			(void)r_hash_do_sha256 (ctx, buf, b);
		}
	}
	r_hash_do_end (ctx, R_HASH_MD5);
	r_hex_bin2str (ctx->digest, R_HASH_SIZE_MD5, hash);

	RList *file_hashes = r_list_newf ((RListFree) r_bin_file_hash_free);
	RBinFileHash *md5h = R_NEW0 (RBinFileHash);
	if (md5h) {
		md5h->type = strdup ("md5");
		md5h->hex = strdup (hash);
		r_list_push (file_hashes, md5h);
	}
	r_hash_do_end (ctx, R_HASH_SHA1);
	r_hex_bin2str (ctx->digest, R_HASH_SIZE_SHA1, hash);

	RBinFileHash *sha1h = R_NEW0 (RBinFileHash);
	if (sha1h) {
		sha1h->type = strdup ("sha1");
		sha1h->hex = strdup (hash);
		r_list_push (file_hashes, sha1h);
	}
	r_hash_do_end (ctx, R_HASH_SHA256);
	r_hex_bin2str (ctx->digest, R_HASH_SIZE_SHA256, hash);

	RBinFileHash *sha256h = R_NEW0 (RBinFileHash);
	if (sha256h) {
		sha256h->type = strdup ("sha256");
		sha256h->hex = strdup (hash);
		r_list_push (file_hashes, sha256h);
	}

	if (o->plugin && o->plugin->hashes) {
		RList *plugin_hashes = o->plugin->hashes (bf);
		r_list_join (file_hashes, plugin_hashes);
		free (plugin_hashes);
	}
	// TODO: add here more rows

	free (buf);
	r_hash_free (ctx);
	return file_hashes;
}