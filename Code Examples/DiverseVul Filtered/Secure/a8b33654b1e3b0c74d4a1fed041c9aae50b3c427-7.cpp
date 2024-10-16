static void serial_unlink_irq_chain(struct mp_port *mtpt)
{
	struct irq_info *i = irq_lists + mtpt->port.irq;

	if (list_empty(i->head))
	{
		free_irq(mtpt->port.irq, i);
	}
	serial_do_unlink(i, mtpt);
}