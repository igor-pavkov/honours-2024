void vga_mem_writeb(VGACommonState *s, hwaddr addr, uint32_t val)
{
    int memory_map_mode, plane, write_mode, b, func_select, mask;
    uint32_t write_mask, bit_mask, set_mask;

#ifdef DEBUG_VGA_MEM
    printf("vga: [0x" TARGET_FMT_plx "] = 0x%02x\n", addr, val);
#endif
    /* convert to VGA memory offset */
    memory_map_mode = (s->gr[VGA_GFX_MISC] >> 2) & 3;
    addr &= 0x1ffff;
    switch(memory_map_mode) {
    case 0:
        break;
    case 1:
        if (addr >= 0x10000)
            return;
        addr += s->bank_offset;
        break;
    case 2:
        addr -= 0x10000;
        if (addr >= 0x8000)
            return;
        break;
    default:
    case 3:
        addr -= 0x18000;
        if (addr >= 0x8000)
            return;
        break;
    }

    if (s->sr[VGA_SEQ_MEMORY_MODE] & VGA_SR04_CHN_4M) {
        /* chain 4 mode : simplest access */
        plane = addr & 3;
        mask = (1 << plane);
        if (s->sr[VGA_SEQ_PLANE_WRITE] & mask) {
            s->vram_ptr[addr] = val;
#ifdef DEBUG_VGA_MEM
            printf("vga: chain4: [0x" TARGET_FMT_plx "]\n", addr);
#endif
            s->plane_updated |= mask; /* only used to detect font change */
            memory_region_set_dirty(&s->vram, addr, 1);
        }
    } else if (s->gr[VGA_GFX_MODE] & 0x10) {
        /* odd/even mode (aka text mode mapping) */
        plane = (s->gr[VGA_GFX_PLANE_READ] & 2) | (addr & 1);
        mask = (1 << plane);
        if (s->sr[VGA_SEQ_PLANE_WRITE] & mask) {
            addr = ((addr & ~1) << 1) | plane;
            s->vram_ptr[addr] = val;
#ifdef DEBUG_VGA_MEM
            printf("vga: odd/even: [0x" TARGET_FMT_plx "]\n", addr);
#endif
            s->plane_updated |= mask; /* only used to detect font change */
            memory_region_set_dirty(&s->vram, addr, 1);
        }
    } else {
        /* standard VGA latched access */
        write_mode = s->gr[VGA_GFX_MODE] & 3;
        switch(write_mode) {
        default:
        case 0:
            /* rotate */
            b = s->gr[VGA_GFX_DATA_ROTATE] & 7;
            val = ((val >> b) | (val << (8 - b))) & 0xff;
            val |= val << 8;
            val |= val << 16;

            /* apply set/reset mask */
            set_mask = mask16[s->gr[VGA_GFX_SR_ENABLE]];
            val = (val & ~set_mask) |
                (mask16[s->gr[VGA_GFX_SR_VALUE]] & set_mask);
            bit_mask = s->gr[VGA_GFX_BIT_MASK];
            break;
        case 1:
            val = s->latch;
            goto do_write;
        case 2:
            val = mask16[val & 0x0f];
            bit_mask = s->gr[VGA_GFX_BIT_MASK];
            break;
        case 3:
            /* rotate */
            b = s->gr[VGA_GFX_DATA_ROTATE] & 7;
            val = (val >> b) | (val << (8 - b));

            bit_mask = s->gr[VGA_GFX_BIT_MASK] & val;
            val = mask16[s->gr[VGA_GFX_SR_VALUE]];
            break;
        }

        /* apply logical operation */
        func_select = s->gr[VGA_GFX_DATA_ROTATE] >> 3;
        switch(func_select) {
        case 0:
        default:
            /* nothing to do */
            break;
        case 1:
            /* and */
            val &= s->latch;
            break;
        case 2:
            /* or */
            val |= s->latch;
            break;
        case 3:
            /* xor */
            val ^= s->latch;
            break;
        }

        /* apply bit mask */
        bit_mask |= bit_mask << 8;
        bit_mask |= bit_mask << 16;
        val = (val & bit_mask) | (s->latch & ~bit_mask);

    do_write:
        /* mask data according to sr[2] */
        mask = s->sr[VGA_SEQ_PLANE_WRITE];
        s->plane_updated |= mask; /* only used to detect font change */
        write_mask = mask16[mask];
        ((uint32_t *)s->vram_ptr)[addr] =
            (((uint32_t *)s->vram_ptr)[addr] & ~write_mask) |
            (val & write_mask);
#ifdef DEBUG_VGA_MEM
        printf("vga: latch: [0x" TARGET_FMT_plx "] mask=0x%08x val=0x%08x\n",
               addr * 4, write_mask, val);
#endif
        memory_region_set_dirty(&s->vram, addr << 2, sizeof(uint32_t));
    }
}