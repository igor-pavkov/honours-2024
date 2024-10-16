static void lan9118_update(lan9118_state *s)
{
    int level;

    /* TODO: Implement FIFO level IRQs.  */
    level = (s->int_sts & s->int_en) != 0;
    if (level) {
        s->irq_cfg |= IRQ_INT;
    } else {
        s->irq_cfg &= ~IRQ_INT;
    }
    if ((s->irq_cfg & IRQ_EN) == 0) {
        level = 0;
    }
    if ((s->irq_cfg & (IRQ_TYPE | IRQ_POL)) != (IRQ_TYPE | IRQ_POL)) {
        /* Interrupt is active low unless we're configured as
         * active-high polarity, push-pull type.
         */
        level = !level;
    }
    qemu_set_irq(s->irq, level);
}