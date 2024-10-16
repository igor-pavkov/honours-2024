void nfc_llcp_sock_free(struct nfc_llcp_sock *sock)
{
	kfree(sock->service_name);

	skb_queue_purge(&sock->tx_queue);
	skb_queue_purge(&sock->tx_pending_queue);

	list_del_init(&sock->accept_queue);

	sock->parent = NULL;

	nfc_llcp_local_put(sock->local);
}