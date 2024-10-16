static int wanxl_puts_command(card_t *card, u32 cmd)
{
	unsigned long timeout = jiffies + 5 * HZ;

	writel(cmd, card->plx + PLX_MAILBOX_1);
	do {
		if (readl(card->plx + PLX_MAILBOX_1) == 0)
			return 0;

		schedule();
	}while (time_after(timeout, jiffies));

	return -1;
}