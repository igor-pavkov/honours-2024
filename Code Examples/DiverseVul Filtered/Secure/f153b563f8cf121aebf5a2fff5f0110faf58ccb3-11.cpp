static void cirrus_mmio_blt_write(CirrusVGAState * s, unsigned address,
				  uint8_t value)
{
    switch (address) {
    case (CIRRUS_MMIO_BLTBGCOLOR + 0):
	cirrus_vga_write_gr(s, 0x00, value);
	break;
    case (CIRRUS_MMIO_BLTBGCOLOR + 1):
	cirrus_vga_write_gr(s, 0x10, value);
	break;
    case (CIRRUS_MMIO_BLTBGCOLOR + 2):
	cirrus_vga_write_gr(s, 0x12, value);
	break;
    case (CIRRUS_MMIO_BLTBGCOLOR + 3):
	cirrus_vga_write_gr(s, 0x14, value);
	break;
    case (CIRRUS_MMIO_BLTFGCOLOR + 0):
	cirrus_vga_write_gr(s, 0x01, value);
	break;
    case (CIRRUS_MMIO_BLTFGCOLOR + 1):
	cirrus_vga_write_gr(s, 0x11, value);
	break;
    case (CIRRUS_MMIO_BLTFGCOLOR + 2):
	cirrus_vga_write_gr(s, 0x13, value);
	break;
    case (CIRRUS_MMIO_BLTFGCOLOR + 3):
	cirrus_vga_write_gr(s, 0x15, value);
	break;
    case (CIRRUS_MMIO_BLTWIDTH + 0):
	cirrus_vga_write_gr(s, 0x20, value);
	break;
    case (CIRRUS_MMIO_BLTWIDTH + 1):
	cirrus_vga_write_gr(s, 0x21, value);
	break;
    case (CIRRUS_MMIO_BLTHEIGHT + 0):
	cirrus_vga_write_gr(s, 0x22, value);
	break;
    case (CIRRUS_MMIO_BLTHEIGHT + 1):
	cirrus_vga_write_gr(s, 0x23, value);
	break;
    case (CIRRUS_MMIO_BLTDESTPITCH + 0):
	cirrus_vga_write_gr(s, 0x24, value);
	break;
    case (CIRRUS_MMIO_BLTDESTPITCH + 1):
	cirrus_vga_write_gr(s, 0x25, value);
	break;
    case (CIRRUS_MMIO_BLTSRCPITCH + 0):
	cirrus_vga_write_gr(s, 0x26, value);
	break;
    case (CIRRUS_MMIO_BLTSRCPITCH + 1):
	cirrus_vga_write_gr(s, 0x27, value);
	break;
    case (CIRRUS_MMIO_BLTDESTADDR + 0):
	cirrus_vga_write_gr(s, 0x28, value);
	break;
    case (CIRRUS_MMIO_BLTDESTADDR + 1):
	cirrus_vga_write_gr(s, 0x29, value);
	break;
    case (CIRRUS_MMIO_BLTDESTADDR + 2):
	cirrus_vga_write_gr(s, 0x2a, value);
	break;
    case (CIRRUS_MMIO_BLTDESTADDR + 3):
	/* ignored */
	break;
    case (CIRRUS_MMIO_BLTSRCADDR + 0):
	cirrus_vga_write_gr(s, 0x2c, value);
	break;
    case (CIRRUS_MMIO_BLTSRCADDR + 1):
	cirrus_vga_write_gr(s, 0x2d, value);
	break;
    case (CIRRUS_MMIO_BLTSRCADDR + 2):
	cirrus_vga_write_gr(s, 0x2e, value);
	break;
    case CIRRUS_MMIO_BLTWRITEMASK:
	cirrus_vga_write_gr(s, 0x2f, value);
	break;
    case CIRRUS_MMIO_BLTMODE:
	cirrus_vga_write_gr(s, 0x30, value);
	break;
    case CIRRUS_MMIO_BLTROP:
	cirrus_vga_write_gr(s, 0x32, value);
	break;
    case CIRRUS_MMIO_BLTMODEEXT:
	cirrus_vga_write_gr(s, 0x33, value);
	break;
    case (CIRRUS_MMIO_BLTTRANSPARENTCOLOR + 0):
	cirrus_vga_write_gr(s, 0x34, value);
	break;
    case (CIRRUS_MMIO_BLTTRANSPARENTCOLOR + 1):
	cirrus_vga_write_gr(s, 0x35, value);
	break;
    case (CIRRUS_MMIO_BLTTRANSPARENTCOLORMASK + 0):
	cirrus_vga_write_gr(s, 0x38, value);
	break;
    case (CIRRUS_MMIO_BLTTRANSPARENTCOLORMASK + 1):
	cirrus_vga_write_gr(s, 0x39, value);
	break;
    case CIRRUS_MMIO_BLTSTATUS:
	cirrus_vga_write_gr(s, 0x31, value);
	break;
    default:
#ifdef DEBUG_CIRRUS
	printf("cirrus: mmio write - addr 0x%04x val 0x%02x (ignored)\n",
	       address, value);
#endif
	break;
    }
}