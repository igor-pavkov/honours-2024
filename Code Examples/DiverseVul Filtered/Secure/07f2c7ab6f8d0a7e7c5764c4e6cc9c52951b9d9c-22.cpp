static struct sctp_chunk *sctp_make_data(const struct sctp_association *asoc,
					 __u8 flags, int paylen, gfp_t gfp)
{
	return _sctp_make_chunk(asoc, SCTP_CID_DATA, flags, paylen, gfp);
}