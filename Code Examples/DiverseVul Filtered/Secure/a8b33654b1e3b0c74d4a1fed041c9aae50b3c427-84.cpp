static void multi_release_std_resource(struct mp_port *mtpt)
{
	unsigned int size = 8 << mtpt->port.regshift;

	switch (mtpt->port.iotype) {
		case UPIO_MEM:
			if (!mtpt->port.mapbase)
				break;

			if (mtpt->port.flags & UPF_IOREMAP) {
				iounmap(mtpt->port.membase);
				mtpt->port.membase = NULL;
			}

			release_mem_region(mtpt->port.mapbase, size);
			break;

		case UPIO_HUB6:
		case UPIO_PORT:
			release_region(mtpt->port.iobase,size);
			break;
	}
}