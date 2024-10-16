static s32 brcmf_set_rts(struct net_device *ndev, u32 rts_threshold)
{
	struct brcmf_if *ifp = netdev_priv(ndev);
	struct wiphy *wiphy = ifp->drvr->wiphy;
	s32 err = 0;

	err = brcmf_fil_iovar_int_set(ifp, "rtsthresh", rts_threshold);
	if (err)
		bphy_err(wiphy, "Error (%d)\n", err);

	return err;
}