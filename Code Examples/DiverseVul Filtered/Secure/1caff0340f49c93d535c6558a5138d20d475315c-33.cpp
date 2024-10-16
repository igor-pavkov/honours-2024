static int e1000_post_load(void *opaque, int version_id)
{
    E1000State *s = opaque;
    NetClientState *nc = qemu_get_queue(s->nic);

    if (!chkflag(MIT)) {
        s->mac_reg[ITR] = s->mac_reg[RDTR] = s->mac_reg[RADV] =
            s->mac_reg[TADV] = 0;
        s->mit_irq_level = false;
    }
    s->mit_ide = 0;
    s->mit_timer_on = true;
    timer_mod(s->mit_timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + 1);

    /* nc.link_down can't be migrated, so infer link_down according
     * to link status bit in mac_reg[STATUS].
     * Alternatively, restart link negotiation if it was in progress. */
    nc->link_down = (s->mac_reg[STATUS] & E1000_STATUS_LU) == 0;

    if (have_autoneg(s) &&
        !(s->phy_reg[PHY_STATUS] & MII_SR_AUTONEG_COMPLETE)) {
        nc->link_down = false;
        timer_mod(s->autoneg_timer,
                  qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL) + 500);
    }

    s->tx.props = s->mig_props;
    if (!s->received_tx_tso) {
        /* We received only one set of offload data (tx.props)
         * and haven't got tx.tso_props.  The best we can do
         * is dupe the data.
         */
        s->tx.tso_props = s->mig_props;
    }
    return 0;
}