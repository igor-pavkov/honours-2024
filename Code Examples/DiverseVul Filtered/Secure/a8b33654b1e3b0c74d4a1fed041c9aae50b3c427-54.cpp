static void serial_do_unlink(struct irq_info *i, struct mp_port *mtpt)
{
	spin_lock_irq(&i->lock);

	if (!list_empty(i->head)) {
		if (i->head == &mtpt->list)
			i->head = i->head->next;
		list_del(&mtpt->list);
	} else {
		i->head = NULL;
	}

	spin_unlock_irq(&i->lock);
}