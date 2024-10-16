static void qrtr_port_put(struct qrtr_sock *ipc)
{
	sock_put(&ipc->sk);
}