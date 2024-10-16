static void sock_def_destruct(struct sock *sk)
{
	kfree(sk->sk_protinfo);
}