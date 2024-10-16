static uint8_t vga_precise_retrace(VGACommonState *s)
{
    struct vga_precise_retrace *r = &s->retrace_info.precise;
    uint8_t val = s->st01 & ~(ST01_V_RETRACE | ST01_DISP_ENABLE);

    if (r->total_chars) {
        int cur_line, cur_line_char, cur_char;
        int64_t cur_tick;

        cur_tick = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);

        cur_char = (cur_tick / r->ticks_per_char) % r->total_chars;
        cur_line = cur_char / r->htotal;

        if (cur_line >= r->vstart && cur_line <= r->vend) {
            val |= ST01_V_RETRACE | ST01_DISP_ENABLE;
        } else {
            cur_line_char = cur_char % r->htotal;
            if (cur_line_char >= r->hstart && cur_line_char <= r->hend) {
                val |= ST01_DISP_ENABLE;
            }
        }

        return val;
    } else {
        return s->st01 ^ (ST01_V_RETRACE | ST01_DISP_ENABLE);
    }
}