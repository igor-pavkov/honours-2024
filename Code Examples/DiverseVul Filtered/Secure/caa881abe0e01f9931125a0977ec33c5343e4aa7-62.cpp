static void pxa2xx_ssp_int_update(PXA2xxSSPState *s)
{
    int level = 0;

    level |= s->ssitr & SSITR_INT;
    level |= (s->sssr & SSSR_BCE)  &&  (s->sscr[1] & SSCR1_EBCEI);
    level |= (s->sssr & SSSR_TUR)  && !(s->sscr[0] & SSCR0_TIM);
    level |= (s->sssr & SSSR_EOC)  &&  (s->sssr & (SSSR_TINT | SSSR_PINT));
    level |= (s->sssr & SSSR_TINT) &&  (s->sscr[1] & SSCR1_TINTE);
    level |= (s->sssr & SSSR_PINT) &&  (s->sscr[1] & SSCR1_PINTE);
    level |= (s->sssr & SSSR_ROR)  && !(s->sscr[0] & SSCR0_RIM);
    level |= (s->sssr & SSSR_RFS)  &&  (s->sscr[1] & SSCR1_RIE);
    level |= (s->sssr & SSSR_TFS)  &&  (s->sscr[1] & SSCR1_TIE);
    qemu_set_irq(s->irq, !!level);
}