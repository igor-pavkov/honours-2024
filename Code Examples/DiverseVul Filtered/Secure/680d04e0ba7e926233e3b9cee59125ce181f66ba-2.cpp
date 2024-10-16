static bool vmci_transport_stream_allow(u32 cid, u32 port)
{
	static const u32 non_socket_contexts[] = {
		VMADDR_CID_HYPERVISOR,
		VMADDR_CID_RESERVED,
	};
	int i;

	BUILD_BUG_ON(sizeof(cid) != sizeof(*non_socket_contexts));

	for (i = 0; i < ARRAY_SIZE(non_socket_contexts); i++) {
		if (cid == non_socket_contexts[i])
			return false;
	}

	return true;
}