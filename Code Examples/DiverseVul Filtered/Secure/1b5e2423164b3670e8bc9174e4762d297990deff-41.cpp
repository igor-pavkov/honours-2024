static s32 brcmf_set_frag(struct net_device *ndev, u32 frag_threshold)
{
	struct brcmf_if *ifp = netdev_priv(ndev);
	struct wiphy *wiphy = ifp->drvr->wiphy;
	s32 err = 0;

	err = brcmf_fil_iovar_int_set(ifp, "fragthresh",
				      frag_threshold);
	if (err)
		bphy_err(wiphy, "Error (%d)\n", err);

	return err;
}