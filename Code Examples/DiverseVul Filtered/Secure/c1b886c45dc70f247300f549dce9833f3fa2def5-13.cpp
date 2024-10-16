uint32_t vga_ioport_read(void *opaque, uint32_t addr)
{
    VGACommonState *s = opaque;
    int val, index;

    if (vga_ioport_invalid(s, addr)) {
        val = 0xff;
    } else {
        switch(addr) {
        case VGA_ATT_W:
            if (s->ar_flip_flop == 0) {
                val = s->ar_index;
            } else {
                val = 0;
            }
            break;
        case VGA_ATT_R:
            index = s->ar_index & 0x1f;
            if (index < VGA_ATT_C) {
                val = s->ar[index];
            } else {
                val = 0;
            }
            break;
        case VGA_MIS_W:
            val = s->st00;
            break;
        case VGA_SEQ_I:
            val = s->sr_index;
            break;
        case VGA_SEQ_D:
            val = s->sr[s->sr_index];
#ifdef DEBUG_VGA_REG
            printf("vga: read SR%x = 0x%02x\n", s->sr_index, val);
#endif
            break;
        case VGA_PEL_IR:
            val = s->dac_state;
            break;
        case VGA_PEL_IW:
            val = s->dac_write_index;
            break;
        case VGA_PEL_D:
            val = s->palette[s->dac_read_index * 3 + s->dac_sub_index];
            if (++s->dac_sub_index == 3) {
                s->dac_sub_index = 0;
                s->dac_read_index++;
            }
            break;
        case VGA_FTC_R:
            val = s->fcr;
            break;
        case VGA_MIS_R:
            val = s->msr;
            break;
        case VGA_GFX_I:
            val = s->gr_index;
            break;
        case VGA_GFX_D:
            val = s->gr[s->gr_index];
#ifdef DEBUG_VGA_REG
            printf("vga: read GR%x = 0x%02x\n", s->gr_index, val);
#endif
            break;
        case VGA_CRT_IM:
        case VGA_CRT_IC:
            val = s->cr_index;
            break;
        case VGA_CRT_DM:
        case VGA_CRT_DC:
            val = s->cr[s->cr_index];
#ifdef DEBUG_VGA_REG
            printf("vga: read CR%x = 0x%02x\n", s->cr_index, val);
#endif
            break;
        case VGA_IS1_RM:
        case VGA_IS1_RC:
            /* just toggle to fool polling */
            val = s->st01 = s->retrace(s);
            s->ar_flip_flop = 0;
            break;
        default:
            val = 0x00;
            break;
        }
    }
#if defined(DEBUG_VGA)
    printf("VGA: read addr=0x%04x data=0x%02x\n", addr, val);
#endif
    return val;
}