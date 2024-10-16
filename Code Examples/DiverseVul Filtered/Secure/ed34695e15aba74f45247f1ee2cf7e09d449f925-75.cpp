static int ff_layout_encode_ioerr(struct xdr_stream *xdr,
				  const struct nfs4_layoutreturn_args *args,
				  const struct nfs4_flexfile_layoutreturn_args *ff_args)
{
	__be32 *start;

	start = xdr_reserve_space(xdr, 4);
	if (unlikely(!start))
		return -E2BIG;

	*start = cpu_to_be32(ff_args->num_errors);
	/* This assume we always return _ALL_ layouts */
	return ff_layout_encode_ds_ioerr(xdr, &ff_args->errors);
}