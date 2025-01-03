void ati_2d_blt(ATIVGAState *s)
{
    /* FIXME it is probably more complex than this and may need to be */
    /* rewritten but for now as a start just to get some output: */
    DisplaySurface *ds = qemu_console_surface(s->vga.con);
    DPRINTF("%p %u ds: %p %d %d rop: %x\n", s->vga.vram_ptr,
            s->vga.vbe_start_addr, surface_data(ds), surface_stride(ds),
            surface_bits_per_pixel(ds),
            (s->regs.dp_mix & GMC_ROP3_MASK) >> 16);
    unsigned dst_x = (s->regs.dp_cntl & DST_X_LEFT_TO_RIGHT ?
                      s->regs.dst_x : s->regs.dst_x + 1 - s->regs.dst_width);
    unsigned dst_y = (s->regs.dp_cntl & DST_Y_TOP_TO_BOTTOM ?
                      s->regs.dst_y : s->regs.dst_y + 1 - s->regs.dst_height);
    int bpp = ati_bpp_from_datatype(s);
    if (!bpp) {
        qemu_log_mask(LOG_GUEST_ERROR, "Invalid bpp\n");
        return;
    }
    int dst_stride = DEFAULT_CNTL ? s->regs.dst_pitch : s->regs.default_pitch;
    if (!dst_stride) {
        qemu_log_mask(LOG_GUEST_ERROR, "Zero dest pitch\n");
        return;
    }
    uint8_t *dst_bits = s->vga.vram_ptr + (DEFAULT_CNTL ?
                        s->regs.dst_offset : s->regs.default_offset);

    if (s->dev_id == PCI_DEVICE_ID_ATI_RAGE128_PF) {
        dst_bits += s->regs.crtc_offset & 0x07ffffff;
        dst_stride *= bpp;
    }
    uint8_t *end = s->vga.vram_ptr + s->vga.vram_size;
    if (dst_bits >= end || dst_bits + dst_x + (dst_y + s->regs.dst_height) *
        dst_stride >= end) {
        qemu_log_mask(LOG_UNIMP, "blt outside vram not implemented\n");
        return;
    }
    DPRINTF("%d %d %d, %d %d %d, (%d,%d) -> (%d,%d) %dx%d %c %c\n",
            s->regs.src_offset, s->regs.dst_offset, s->regs.default_offset,
            s->regs.src_pitch, s->regs.dst_pitch, s->regs.default_pitch,
            s->regs.src_x, s->regs.src_y, s->regs.dst_x, s->regs.dst_y,
            s->regs.dst_width, s->regs.dst_height,
            (s->regs.dp_cntl & DST_X_LEFT_TO_RIGHT ? '>' : '<'),
            (s->regs.dp_cntl & DST_Y_TOP_TO_BOTTOM ? 'v' : '^'));
    switch (s->regs.dp_mix & GMC_ROP3_MASK) {
    case ROP3_SRCCOPY:
    {
        unsigned src_x = (s->regs.dp_cntl & DST_X_LEFT_TO_RIGHT ?
                       s->regs.src_x : s->regs.src_x + 1 - s->regs.dst_width);
        unsigned src_y = (s->regs.dp_cntl & DST_Y_TOP_TO_BOTTOM ?
                       s->regs.src_y : s->regs.src_y + 1 - s->regs.dst_height);
        int src_stride = DEFAULT_CNTL ?
                         s->regs.src_pitch : s->regs.default_pitch;
        if (!src_stride) {
            qemu_log_mask(LOG_GUEST_ERROR, "Zero source pitch\n");
            return;
        }
        uint8_t *src_bits = s->vga.vram_ptr + (DEFAULT_CNTL ?
                            s->regs.src_offset : s->regs.default_offset);

        if (s->dev_id == PCI_DEVICE_ID_ATI_RAGE128_PF) {
            src_bits += s->regs.crtc_offset & 0x07ffffff;
            src_stride *= bpp;
        }
        if (src_bits >= end || src_bits + src_x +
            (src_y + s->regs.dst_height) * src_stride >= end) {
            qemu_log_mask(LOG_UNIMP, "blt outside vram not implemented\n");
            return;
        }

        src_stride /= sizeof(uint32_t);
        dst_stride /= sizeof(uint32_t);
        DPRINTF("pixman_blt(%p, %p, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)\n",
                src_bits, dst_bits, src_stride, dst_stride, bpp, bpp,
                src_x, src_y, dst_x, dst_y,
                s->regs.dst_width, s->regs.dst_height);
        if (s->regs.dp_cntl & DST_X_LEFT_TO_RIGHT &&
            s->regs.dp_cntl & DST_Y_TOP_TO_BOTTOM) {
            pixman_blt((uint32_t *)src_bits, (uint32_t *)dst_bits,
                       src_stride, dst_stride, bpp, bpp,
                       src_x, src_y, dst_x, dst_y,
                       s->regs.dst_width, s->regs.dst_height);
        } else {
            /* FIXME: We only really need a temporary if src and dst overlap */
            int llb = s->regs.dst_width * (bpp / 8);
            int tmp_stride = DIV_ROUND_UP(llb, sizeof(uint32_t));
            uint32_t *tmp = g_malloc(tmp_stride * sizeof(uint32_t) *
                                     s->regs.dst_height);
            pixman_blt((uint32_t *)src_bits, tmp,
                       src_stride, tmp_stride, bpp, bpp,
                       src_x, src_y, 0, 0,
                       s->regs.dst_width, s->regs.dst_height);
            pixman_blt(tmp, (uint32_t *)dst_bits,
                       tmp_stride, dst_stride, bpp, bpp,
                       0, 0, dst_x, dst_y,
                       s->regs.dst_width, s->regs.dst_height);
            g_free(tmp);
        }
        if (dst_bits >= s->vga.vram_ptr + s->vga.vbe_start_addr &&
            dst_bits < s->vga.vram_ptr + s->vga.vbe_start_addr +
            s->vga.vbe_regs[VBE_DISPI_INDEX_YRES] * s->vga.vbe_line_offset) {
            memory_region_set_dirty(&s->vga.vram, s->vga.vbe_start_addr +
                                    s->regs.dst_offset +
                                    dst_y * surface_stride(ds),
                                    s->regs.dst_height * surface_stride(ds));
        }
        s->regs.dst_x = (s->regs.dp_cntl & DST_X_LEFT_TO_RIGHT ?
                         dst_x + s->regs.dst_width : dst_x);
        s->regs.dst_y = (s->regs.dp_cntl & DST_Y_TOP_TO_BOTTOM ?
                         dst_y + s->regs.dst_height : dst_y);
        break;
    }
    case ROP3_PATCOPY:
    case ROP3_BLACKNESS:
    case ROP3_WHITENESS:
    {
        uint32_t filler = 0;

        switch (s->regs.dp_mix & GMC_ROP3_MASK) {
        case ROP3_PATCOPY:
            filler = s->regs.dp_brush_frgd_clr;
            break;
        case ROP3_BLACKNESS:
            filler = 0xffUL << 24 | rgb_to_pixel32(s->vga.palette[0],
                     s->vga.palette[1], s->vga.palette[2]);
            break;
        case ROP3_WHITENESS:
            filler = 0xffUL << 24 | rgb_to_pixel32(s->vga.palette[3],
                     s->vga.palette[4], s->vga.palette[5]);
            break;
        }

        dst_stride /= sizeof(uint32_t);
        DPRINTF("pixman_fill(%p, %d, %d, %d, %d, %d, %d, %x)\n",
                dst_bits, dst_stride, bpp,
                s->regs.dst_x, s->regs.dst_y,
                s->regs.dst_width, s->regs.dst_height,
                filler);
        pixman_fill((uint32_t *)dst_bits, dst_stride, bpp,
                    s->regs.dst_x, s->regs.dst_y,
                    s->regs.dst_width, s->regs.dst_height,
                    filler);
        if (dst_bits >= s->vga.vram_ptr + s->vga.vbe_start_addr &&
            dst_bits < s->vga.vram_ptr + s->vga.vbe_start_addr +
            s->vga.vbe_regs[VBE_DISPI_INDEX_YRES] * s->vga.vbe_line_offset) {
            memory_region_set_dirty(&s->vga.vram, s->vga.vbe_start_addr +
                                    s->regs.dst_offset +
                                    dst_y * surface_stride(ds),
                                    s->regs.dst_height * surface_stride(ds));
        }
        s->regs.dst_y = (s->regs.dp_cntl & DST_Y_TOP_TO_BOTTOM ?
                         dst_y + s->regs.dst_height : dst_y);
        break;
    }
    default:
        qemu_log_mask(LOG_UNIMP, "Unimplemented ati_2d blt op %x\n",
                      (s->regs.dp_mix & GMC_ROP3_MASK) >> 16);
    }
}