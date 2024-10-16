int ssl3_cbc_remove_padding(const SSL* s,
			    SSL3_RECORD *rec,
			    unsigned block_size,
			    unsigned mac_size)
	{
	unsigned padding_length, good;
	const unsigned overhead = 1 /* padding length byte */ + mac_size;

	/* These lengths are all public so we can test them in non-constant
	 * time. */
	if (overhead > rec->length)
		return 0;

	padding_length = rec->data[rec->length-1];
	good = constant_time_ge(rec->length, padding_length+overhead);
	/* SSLv3 requires that the padding is minimal. */
	good &= constant_time_ge(block_size, padding_length+1);
	padding_length = good & (padding_length+1);
	rec->length -= padding_length;
	rec->type |= padding_length<<8;	/* kludge: pass padding length */
	return (int)((good & 1) | (~good & -1));
}