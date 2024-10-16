static void nbt_name_socket_handler(struct tevent_context *ev, struct tevent_fd *fde,
				    uint16_t flags, void *private_data)
{
	struct nbt_name_socket *nbtsock = talloc_get_type(private_data,
							  struct nbt_name_socket);
	if (flags & TEVENT_FD_WRITE) {
		nbt_name_socket_send(nbtsock);
	}
	if (flags & TEVENT_FD_READ) {
		nbt_name_socket_recv(nbtsock);
	}
}