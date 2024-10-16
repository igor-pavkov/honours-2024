static void llcp_sock_destruct(struct sock *sk)
{
	struct nfc_llcp_sock *llcp_sock = nfc_llcp_sock(sk);

	pr_debug("%p\n", sk);

	if (sk->sk_state == LLCP_CONNECTED)
		nfc_put_device(llcp_sock->dev);

	skb_queue_purge(&sk->sk_receive_queue);

	nfc_llcp_sock_free(llcp_sock);

	if (!sock_flag(sk, SOCK_DEAD)) {
		pr_err("Freeing alive NFC LLCP socket %p\n", sk);
		return;
	}
}