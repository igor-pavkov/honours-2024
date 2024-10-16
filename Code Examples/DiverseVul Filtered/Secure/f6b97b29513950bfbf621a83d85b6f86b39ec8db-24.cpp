static unsigned short nr_find_next_circuit(void)
{
	unsigned short id = circuit;
	unsigned char i, j;
	struct sock *sk;

	for (;;) {
		i = id / 256;
		j = id % 256;

		if (i != 0 && j != 0) {
			if ((sk=nr_find_socket(i, j)) == NULL)
				break;
			bh_unlock_sock(sk);
		}

		id++;
	}

	return id;
}