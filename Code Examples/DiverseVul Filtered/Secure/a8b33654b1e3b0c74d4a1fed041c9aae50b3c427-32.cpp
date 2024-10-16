static int serial_link_irq_chain(struct mp_port *mtpt)
{
	struct irq_info *i = irq_lists + mtpt->port.irq;
	int ret, irq_flags = mtpt->port.flags & UPF_SHARE_IRQ ? IRQF_SHARED : 0;
	spin_lock_irq(&i->lock);

	if (i->head) {
		list_add(&mtpt->list, i->head);
		spin_unlock_irq(&i->lock);

		ret = 0;
	} else {
		INIT_LIST_HEAD(&mtpt->list);
		i->head = &mtpt->list;
		spin_unlock_irq(&i->lock);

		ret = request_irq(mtpt->port.irq, multi_interrupt,
				irq_flags, "serial", i);
		if (ret < 0)
			serial_do_unlink(i, mtpt);
	}

	return ret;
}