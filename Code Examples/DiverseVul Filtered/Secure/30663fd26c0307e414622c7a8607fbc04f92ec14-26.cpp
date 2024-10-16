static void gen_lea_v_seg(DisasContext *s, TCGMemOp aflag, TCGv a0,
                          int def_seg, int ovr_seg)
{
    switch (aflag) {
#ifdef TARGET_X86_64
    case MO_64:
        if (ovr_seg < 0) {
            tcg_gen_mov_tl(cpu_A0, a0);
            return;
        }
        break;
#endif
    case MO_32:
        /* 32 bit address */
        if (ovr_seg < 0 && s->addseg) {
            ovr_seg = def_seg;
        }
        if (ovr_seg < 0) {
            tcg_gen_ext32u_tl(cpu_A0, a0);
            return;
        }
        break;
    case MO_16:
        /* 16 bit address */
        tcg_gen_ext16u_tl(cpu_A0, a0);
        a0 = cpu_A0;
        if (ovr_seg < 0) {
            if (s->addseg) {
                ovr_seg = def_seg;
            } else {
                return;
            }
        }
        break;
    default:
        tcg_abort();
    }

    if (ovr_seg >= 0) {
        TCGv seg = cpu_seg_base[ovr_seg];

        if (aflag == MO_64) {
            tcg_gen_add_tl(cpu_A0, a0, seg);
        } else if (CODE64(s)) {
            tcg_gen_ext32u_tl(cpu_A0, a0);
            tcg_gen_add_tl(cpu_A0, cpu_A0, seg);
        } else {
            tcg_gen_add_tl(cpu_A0, a0, seg);
            tcg_gen_ext32u_tl(cpu_A0, cpu_A0);
        }
    }
}