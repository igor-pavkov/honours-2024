static struct sock *ec_listening_socket(unsigned char port, unsigned char
				 station, unsigned char net)
{
	struct sock *sk;
	struct hlist_node *node;

	sk_for_each(sk, node, &econet_sklist) {
		struct econet_sock *opt = ec_sk(sk);
		if ((opt->port == port || opt->port == 0) &&
		    (opt->station == station || opt->station == 0) &&
		    (opt->net == net || opt->net == 0))
			goto found;
	}
	sk = NULL;
found:
	return sk;
}