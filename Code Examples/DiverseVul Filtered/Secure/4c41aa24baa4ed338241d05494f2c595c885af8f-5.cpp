static inline void ncp_add_dword_lh(struct ncp_server *server, __u32 x) {
	ncp_add_dword(server, cpu_to_le32(x));
}