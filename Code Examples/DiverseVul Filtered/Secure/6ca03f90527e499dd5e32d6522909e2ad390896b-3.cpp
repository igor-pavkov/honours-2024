static void puts_queue(struct vc_data *vc, char *cp)
{
	while (*cp) {
		tty_insert_flip_char(&vc->port, *cp, 0);
		cp++;
	}
	tty_schedule_flip(&vc->port);
}