static int decode_pnfs_stateid(struct xdr_stream *xdr, nfs4_stateid *stateid)
{
	__be32 *p;

	p = xdr_inline_decode(xdr, NFS4_STATEID_SIZE);
	if (unlikely(p == NULL))
		return -ENOBUFS;
	stateid->type = NFS4_PNFS_DS_STATEID_TYPE;
	memcpy(stateid->data, p, NFS4_STATEID_SIZE);
	dprintk("%s: stateid id= [%x%x%x%x]\n", __func__,
		p[0], p[1], p[2], p[3]);
	return 0;
}