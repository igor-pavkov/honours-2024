int snd_seq_queue_alloc(int client, int locked, unsigned int info_flags)
{
	struct snd_seq_queue *q;

	q = queue_new(client, locked);
	if (q == NULL)
		return -ENOMEM;
	q->info_flags = info_flags;
	if (queue_list_add(q) < 0) {
		queue_delete(q);
		return -ENOMEM;
	}
	snd_seq_queue_use(q->queue, client, 1); /* use this queue */
	return q->queue;
}