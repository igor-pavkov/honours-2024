static int decode_deviceid(struct xdr_stream *xdr, struct nfs4_deviceid *devid)
{
	__be32 *p;

	p = xdr_inline_decode(xdr, NFS4_DEVICEID4_SIZE);
	if (unlikely(!p))
		return -ENOBUFS;
	memcpy(devid, p, NFS4_DEVICEID4_SIZE);
	nfs4_print_deviceid(devid);
	return 0;
}