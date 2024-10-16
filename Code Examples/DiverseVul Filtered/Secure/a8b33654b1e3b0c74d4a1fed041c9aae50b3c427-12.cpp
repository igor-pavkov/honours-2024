static void mp_tasklet_action(unsigned long data)
{
	struct sb_uart_state *state = (struct sb_uart_state *)data;
	struct tty_struct *tty;

	printk("tasklet is called!\n");
	tty = state->info->tty;
	tty_wakeup(tty);
}