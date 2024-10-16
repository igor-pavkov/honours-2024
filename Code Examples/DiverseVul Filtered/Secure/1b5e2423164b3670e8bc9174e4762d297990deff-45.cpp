void brcmf_set_mpc(struct brcmf_if *ifp, int mpc)
{
	struct wiphy *wiphy = ifp->drvr->wiphy;
	s32 err = 0;

	if (check_vif_up(ifp->vif)) {
		err = brcmf_fil_iovar_int_set(ifp, "mpc", mpc);
		if (err) {
			bphy_err(wiphy, "fail to set mpc\n");
			return;
		}
		brcmf_dbg(INFO, "MPC : %d\n", mpc);
	}
}