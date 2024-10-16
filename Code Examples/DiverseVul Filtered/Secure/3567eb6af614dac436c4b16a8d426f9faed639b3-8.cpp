static inline int check_access(struct snd_seq_queue *q, int client)
{
	return (q->owner == client) || (!q->locked && !q->klocked);
}