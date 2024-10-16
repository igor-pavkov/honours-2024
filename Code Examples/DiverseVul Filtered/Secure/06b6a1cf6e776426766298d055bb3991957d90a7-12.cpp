static void rds_recv_incoming_exthdrs(struct rds_incoming *inc, struct rds_sock *rs)
{
	struct rds_header *hdr = &inc->i_hdr;
	unsigned int pos = 0, type, len;
	union {
		struct rds_ext_header_version version;
		struct rds_ext_header_rdma rdma;
		struct rds_ext_header_rdma_dest rdma_dest;
	} buffer;

	while (1) {
		len = sizeof(buffer);
		type = rds_message_next_extension(hdr, &pos, &buffer, &len);
		if (type == RDS_EXTHDR_NONE)
			break;
		/* Process extension header here */
		switch (type) {
		case RDS_EXTHDR_RDMA:
			rds_rdma_unuse(rs, be32_to_cpu(buffer.rdma.h_rdma_rkey), 0);
			break;

		case RDS_EXTHDR_RDMA_DEST:
			/* We ignore the size for now. We could stash it
			 * somewhere and use it for error checking. */
			inc->i_rdma_cookie = rds_rdma_make_cookie(
					be32_to_cpu(buffer.rdma_dest.h_rdma_rkey),
					be32_to_cpu(buffer.rdma_dest.h_rdma_offset));

			break;
		}
	}
}