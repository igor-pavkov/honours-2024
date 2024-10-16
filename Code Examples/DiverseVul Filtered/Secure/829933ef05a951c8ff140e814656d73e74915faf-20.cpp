bus_reset_irq_handler(struct pcilynx *lynx)
{
	struct client *client;
	struct timespec64 ts64;
	u32    timestamp;

	ktime_get_real_ts64(&ts64);
	timestamp = ts64.tv_nsec / NSEC_PER_USEC;

	spin_lock(&lynx->client_list_lock);

	list_for_each_entry(client, &lynx->client_list, link)
		packet_buffer_put(&client->buffer, &timestamp, 4);

	spin_unlock(&lynx->client_list_lock);
}