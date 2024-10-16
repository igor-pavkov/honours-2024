void ide_ioport_write(void *opaque, uint32_t addr, uint32_t val)
{
    IDEBus *bus = opaque;

#ifdef DEBUG_IDE
    printf("IDE: write addr=0x%x val=0x%02x\n", addr, val);
#endif

    addr &= 7;

    /* ignore writes to command block while busy with previous command */
    if (addr != 7 && (idebus_active_if(bus)->status & (BUSY_STAT|DRQ_STAT)))
        return;

    switch(addr) {
    case 0:
        break;
    case 1:
	ide_clear_hob(bus);
        /* NOTE: data is written to the two drives */
	bus->ifs[0].hob_feature = bus->ifs[0].feature;
	bus->ifs[1].hob_feature = bus->ifs[1].feature;
        bus->ifs[0].feature = val;
        bus->ifs[1].feature = val;
        break;
    case 2:
	ide_clear_hob(bus);
	bus->ifs[0].hob_nsector = bus->ifs[0].nsector;
	bus->ifs[1].hob_nsector = bus->ifs[1].nsector;
        bus->ifs[0].nsector = val;
        bus->ifs[1].nsector = val;
        break;
    case 3:
	ide_clear_hob(bus);
	bus->ifs[0].hob_sector = bus->ifs[0].sector;
	bus->ifs[1].hob_sector = bus->ifs[1].sector;
        bus->ifs[0].sector = val;
        bus->ifs[1].sector = val;
        break;
    case 4:
	ide_clear_hob(bus);
	bus->ifs[0].hob_lcyl = bus->ifs[0].lcyl;
	bus->ifs[1].hob_lcyl = bus->ifs[1].lcyl;
        bus->ifs[0].lcyl = val;
        bus->ifs[1].lcyl = val;
        break;
    case 5:
	ide_clear_hob(bus);
	bus->ifs[0].hob_hcyl = bus->ifs[0].hcyl;
	bus->ifs[1].hob_hcyl = bus->ifs[1].hcyl;
        bus->ifs[0].hcyl = val;
        bus->ifs[1].hcyl = val;
        break;
    case 6:
	/* FIXME: HOB readback uses bit 7 */
        bus->ifs[0].select = (val & ~0x10) | 0xa0;
        bus->ifs[1].select = (val | 0x10) | 0xa0;
        /* select drive */
        bus->unit = (val >> 4) & 1;
        break;
    default:
    case 7:
        /* command */
        ide_exec_cmd(bus, val);
        break;
    }
}