static int hulusb_set_channel(struct ieee802154_hw *hw, u8 page, u8 channel)
{
	int rc;
	int rssi_base_val;

	struct atusb *lp = hw->priv;

	if (channel == 0)
		rc = atusb_write_subreg(lp, SR_SUB_MODE, 0);
	else
		rc = atusb_write_subreg(lp, SR_SUB_MODE, 1);
	if (rc < 0)
		return rc;

	if (page == 0) {
		rc = atusb_write_subreg(lp, SR_BPSK_QPSK, 0);
		rssi_base_val = -100;
	} else {
		rc = atusb_write_subreg(lp, SR_BPSK_QPSK, 1);
		rssi_base_val = -98;
	}
	if (rc < 0)
		return rc;

	rc = hulusb_set_cca_ed_level(lp, rssi_base_val);
	if (rc < 0)
		return rc;

	/* This sets the symbol_duration according frequency on the 212.
	 * TODO move this handling while set channel and page in cfg802154.
	 * We can do that, this timings are according 802.15.4 standard.
	 * If we do that in cfg802154, this is a more generic calculation.
	 *
	 * This should also protected from ifs_timer. Means cancel timer and
	 * init with a new value. For now, this is okay.
	 */
	if (channel == 0) {
		if (page == 0) {
			/* SUB:0 and BPSK:0 -> BPSK-20 */
			lp->hw->phy->symbol_duration = 50;
		} else {
			/* SUB:1 and BPSK:0 -> BPSK-40 */
			lp->hw->phy->symbol_duration = 25;
		}
	} else {
		if (page == 0)
			/* SUB:0 and BPSK:1 -> OQPSK-100/200/400 */
			lp->hw->phy->symbol_duration = 40;
		else
			/* SUB:1 and BPSK:1 -> OQPSK-250/500/1000 */
			lp->hw->phy->symbol_duration = 16;
	}

	lp->hw->phy->lifs_period = IEEE802154_LIFS_PERIOD *
				   lp->hw->phy->symbol_duration;
	lp->hw->phy->sifs_period = IEEE802154_SIFS_PERIOD *
				   lp->hw->phy->symbol_duration;

	return atusb_write_subreg(lp, SR_CHANNEL, channel);
}