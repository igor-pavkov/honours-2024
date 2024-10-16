static inline __u32 ncp_reply_dword_lh(struct ncp_server* server, int offset) {
	return le32_to_cpu(ncp_reply_dword(server, offset));
}