static void ff_layout_encode_iostats_array(struct xdr_stream *xdr,
		const struct nfs4_layoutreturn_args *args,
		struct nfs4_flexfile_layoutreturn_args *ff_args)
{
	__be32 *p;
	int i;

	p = xdr_reserve_space(xdr, 4);
	*p = cpu_to_be32(ff_args->num_dev);
	for (i = 0; i < ff_args->num_dev; i++)
		ff_layout_encode_ff_iostat(xdr,
				&args->layout->plh_stateid,
				&ff_args->devinfo[i]);
}