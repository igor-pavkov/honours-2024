static uint8_t cirrus_mmio_blt_read(CirrusVGAState * s, unsigned address)
{
    int value = 0xff;

    switch (address) {
    case (CIRRUS_MMIO_BLTBGCOLOR + 0):
	value = cirrus_vga_read_gr(s, 0x00);
	break;
    case (CIRRUS_MMIO_BLTBGCOLOR + 1):
	value = cirrus_vga_read_gr(s, 0x10);
	break;
    case (CIRRUS_MMIO_BLTBGCOLOR + 2):
	value = cirrus_vga_read_gr(s, 0x12);
	break;
    case (CIRRUS_MMIO_BLTBGCOLOR + 3):
	value = cirrus_vga_read_gr(s, 0x14);
	break;
    case (CIRRUS_MMIO_BLTFGCOLOR + 0):
	value = cirrus_vga_read_gr(s, 0x01);
	break;
    case (CIRRUS_MMIO_BLTFGCOLOR + 1):
	value = cirrus_vga_read_gr(s, 0x11);
	break;
    case (CIRRUS_MMIO_BLTFGCOLOR + 2):
	value = cirrus_vga_read_gr(s, 0x13);
	break;
    case (CIRRUS_MMIO_BLTFGCOLOR + 3):
	value = cirrus_vga_read_gr(s, 0x15);
	break;
    case (CIRRUS_MMIO_BLTWIDTH + 0):
	value = cirrus_vga_read_gr(s, 0x20);
	break;
    case (CIRRUS_MMIO_BLTWIDTH + 1):
	value = cirrus_vga_read_gr(s, 0x21);
	break;
    case (CIRRUS_MMIO_BLTHEIGHT + 0):
	value = cirrus_vga_read_gr(s, 0x22);
	break;
    case (CIRRUS_MMIO_BLTHEIGHT + 1):
	value = cirrus_vga_read_gr(s, 0x23);
	break;
    case (CIRRUS_MMIO_BLTDESTPITCH + 0):
	value = cirrus_vga_read_gr(s, 0x24);
	break;
    case (CIRRUS_MMIO_BLTDESTPITCH + 1):
	value = cirrus_vga_read_gr(s, 0x25);
	break;
    case (CIRRUS_MMIO_BLTSRCPITCH + 0):
	value = cirrus_vga_read_gr(s, 0x26);
	break;
    case (CIRRUS_MMIO_BLTSRCPITCH + 1):
	value = cirrus_vga_read_gr(s, 0x27);
	break;
    case (CIRRUS_MMIO_BLTDESTADDR + 0):
	value = cirrus_vga_read_gr(s, 0x28);
	break;
    case (CIRRUS_MMIO_BLTDESTADDR + 1):
	value = cirrus_vga_read_gr(s, 0x29);
	break;
    case (CIRRUS_MMIO_BLTDESTADDR + 2):
	value = cirrus_vga_read_gr(s, 0x2a);
	break;
    case (CIRRUS_MMIO_BLTSRCADDR + 0):
	value = cirrus_vga_read_gr(s, 0x2c);
	break;
    case (CIRRUS_MMIO_BLTSRCADDR + 1):
	value = cirrus_vga_read_gr(s, 0x2d);
	break;
    case (CIRRUS_MMIO_BLTSRCADDR + 2):
	value = cirrus_vga_read_gr(s, 0x2e);
	break;
    case CIRRUS_MMIO_BLTWRITEMASK:
	value = cirrus_vga_read_gr(s, 0x2f);
	break;
    case CIRRUS_MMIO_BLTMODE:
	value = cirrus_vga_read_gr(s, 0x30);
	break;
    case CIRRUS_MMIO_BLTROP:
	value = cirrus_vga_read_gr(s, 0x32);
	break;
    case CIRRUS_MMIO_BLTMODEEXT:
	value = cirrus_vga_read_gr(s, 0x33);
	break;
    case (CIRRUS_MMIO_BLTTRANSPARENTCOLOR + 0):
	value = cirrus_vga_read_gr(s, 0x34);
	break;
    case (CIRRUS_MMIO_BLTTRANSPARENTCOLOR + 1):
	value = cirrus_vga_read_gr(s, 0x35);
	break;
    case (CIRRUS_MMIO_BLTTRANSPARENTCOLORMASK + 0):
	value = cirrus_vga_read_gr(s, 0x38);
	break;
    case (CIRRUS_MMIO_BLTTRANSPARENTCOLORMASK + 1):
	value = cirrus_vga_read_gr(s, 0x39);
	break;
    case CIRRUS_MMIO_BLTSTATUS:
	value = cirrus_vga_read_gr(s, 0x31);
	break;
    default:
#ifdef DEBUG_CIRRUS
	printf("cirrus: mmio read - address 0x%04x\n", address);
#endif
	break;
    }

    return (uint8_t) value;
}