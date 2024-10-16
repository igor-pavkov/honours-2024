static void cirrus_vga_ioport_write(void *opaque, hwaddr addr, uint64_t val,
                                    unsigned size)
{
    CirrusVGAState *c = opaque;
    VGACommonState *s = &c->vga;
    int index;

    addr += 0x3b0;

    /* check port range access depending on color/monochrome mode */
    if (vga_ioport_invalid(s, addr)) {
	return;
    }
#ifdef DEBUG_VGA
    printf("VGA: write addr=0x%04x data=0x%02x\n", addr, val);
#endif

    switch (addr) {
    case 0x3c0:
	if (s->ar_flip_flop == 0) {
	    val &= 0x3f;
	    s->ar_index = val;
	} else {
	    index = s->ar_index & 0x1f;
	    switch (index) {
	    case 0x00 ... 0x0f:
		s->ar[index] = val & 0x3f;
		break;
	    case 0x10:
		s->ar[index] = val & ~0x10;
		break;
	    case 0x11:
		s->ar[index] = val;
		break;
	    case 0x12:
		s->ar[index] = val & ~0xc0;
		break;
	    case 0x13:
		s->ar[index] = val & ~0xf0;
		break;
	    case 0x14:
		s->ar[index] = val & ~0xf0;
		break;
	    default:
		break;
	    }
	}
	s->ar_flip_flop ^= 1;
	break;
    case 0x3c2:
	s->msr = val & ~0x10;
	s->update_retrace_info(s);
	break;
    case 0x3c4:
	s->sr_index = val;
	break;
    case 0x3c5:
#ifdef DEBUG_VGA_REG
	printf("vga: write SR%x = 0x%02" PRIu64 "\n", s->sr_index, val);
#endif
	cirrus_vga_write_sr(c, val);
        break;
    case 0x3c6:
	cirrus_write_hidden_dac(c, val);
	break;
    case 0x3c7:
	s->dac_read_index = val;
	s->dac_sub_index = 0;
	s->dac_state = 3;
	break;
    case 0x3c8:
	s->dac_write_index = val;
	s->dac_sub_index = 0;
	s->dac_state = 0;
	break;
    case 0x3c9:
        cirrus_vga_write_palette(c, val);
        break;
    case 0x3ce:
	s->gr_index = val;
	break;
    case 0x3cf:
#ifdef DEBUG_VGA_REG
	printf("vga: write GR%x = 0x%02" PRIu64 "\n", s->gr_index, val);
#endif
	cirrus_vga_write_gr(c, s->gr_index, val);
	break;
    case 0x3b4:
    case 0x3d4:
	s->cr_index = val;
	break;
    case 0x3b5:
    case 0x3d5:
#ifdef DEBUG_VGA_REG
	printf("vga: write CR%x = 0x%02"PRIu64"\n", s->cr_index, val);
#endif
	cirrus_vga_write_cr(c, val);
	break;
    case 0x3ba:
    case 0x3da:
	s->fcr = val & 0x10;
	break;
    }
}