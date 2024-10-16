static void fn_send_intr(struct vc_data *vc)
{
	tty_insert_flip_char(&vc->port, 0, TTY_BREAK);
	tty_schedule_flip(&vc->port);
}