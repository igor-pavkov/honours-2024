NTSTATUS nbt_set_unexpected_handler(struct nbt_name_socket *nbtsock,
				    void (*handler)(struct nbt_name_socket *, struct nbt_name_packet *,
						    struct socket_address *),
				    void *private_data)
{
	nbtsock->unexpected.handler = handler;
	nbtsock->unexpected.private_data = private_data;
	TEVENT_FD_READABLE(nbtsock->fde);
	return NT_STATUS_OK;
}