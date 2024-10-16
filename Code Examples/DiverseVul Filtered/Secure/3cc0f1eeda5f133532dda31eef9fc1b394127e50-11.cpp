_PUBLIC_ struct nbt_name_socket *nbt_name_socket_init(TALLOC_CTX *mem_ctx,
					     struct tevent_context *event_ctx)
{
	struct nbt_name_socket *nbtsock;
	NTSTATUS status;

	nbtsock = talloc(mem_ctx, struct nbt_name_socket);
	if (nbtsock == NULL) goto failed;

	nbtsock->event_ctx = event_ctx;
	if (nbtsock->event_ctx == NULL) goto failed;

	status = socket_create(nbtsock, "ip", SOCKET_TYPE_DGRAM,
			       &nbtsock->sock, 0);
	if (!NT_STATUS_IS_OK(status)) goto failed;

	socket_set_option(nbtsock->sock, "SO_BROADCAST", "1");

	nbtsock->idr = idr_init(nbtsock);
	if (nbtsock->idr == NULL) goto failed;

	nbtsock->send_queue = NULL;
	nbtsock->num_pending = 0;
	nbtsock->incoming.handler = NULL;
	nbtsock->unexpected.handler = NULL;

	nbtsock->fde = tevent_add_fd(nbtsock->event_ctx, nbtsock,
				     socket_get_fd(nbtsock->sock), 0,
				     nbt_name_socket_handler, nbtsock);

	return nbtsock;

failed:
	talloc_free(nbtsock);
	return NULL;
}