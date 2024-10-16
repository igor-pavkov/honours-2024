static int e1000_pre_save(void *opaque)
{
    E1000State *s = opaque;
    NetClientState *nc = qemu_get_queue(s->nic);

    /*
     * If link is down and auto-negotiation is supported and ongoing,
     * complete auto-negotiation immediately. This allows us to look
     * at MII_SR_AUTONEG_COMPLETE to infer link status on load.
     */
    if (nc->link_down && have_autoneg(s)) {
        s->phy_reg[PHY_STATUS] |= MII_SR_AUTONEG_COMPLETE;
    }

    /* Decide which set of props to migrate in the main structure */
    if (chkflag(TSO) || !s->use_tso_for_migration) {
        /* Either we're migrating with the extra subsection, in which
         * case the mig_props is always 'props' OR
         * we've not got the subsection, but 'props' was the last
         * updated.
         */
        s->mig_props = s->tx.props;
    } else {
        /* We're not using the subsection, and 'tso_props' was
         * the last updated.
         */
        s->mig_props = s->tx.tso_props;
    }
    return 0;
}