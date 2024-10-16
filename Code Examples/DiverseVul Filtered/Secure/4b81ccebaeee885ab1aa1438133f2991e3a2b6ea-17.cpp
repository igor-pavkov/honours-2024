static void bpf_ringbuf_notify(struct irq_work *work)
{
	struct bpf_ringbuf *rb = container_of(work, struct bpf_ringbuf, work);

	wake_up_all(&rb->waitq);
}