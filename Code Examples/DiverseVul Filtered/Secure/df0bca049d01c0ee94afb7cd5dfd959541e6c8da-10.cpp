static void release_proto_idx(struct proto *prot)
{
	if (prot->inuse_idx != PROTO_INUSE_NR - 1)
		clear_bit(prot->inuse_idx, proto_inuse_idx);
}