static void pipe_destruct(struct sock *sk)
{
	struct pep_sock *pn = pep_sk(sk);

	skb_queue_purge(&sk->sk_receive_queue);
	skb_queue_purge(&pn->ctrlreq_queue);
}