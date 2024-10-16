static void put_queue(struct vc_data *vc, int ch)
{
	tty_insert_flip_char(&vc->port, ch, 0);
	tty_schedule_flip(&vc->port);
}