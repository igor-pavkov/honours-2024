static s32 brcmf_set_retry(struct net_device *ndev, u32 retry, bool l)
{
	struct brcmf_if *ifp = netdev_priv(ndev);
	struct wiphy *wiphy = ifp->drvr->wiphy;
	s32 err = 0;
	u32 cmd = (l ? BRCMF_C_SET_LRL : BRCMF_C_SET_SRL);

	err = brcmf_fil_cmd_int_set(ifp, cmd, retry);
	if (err) {
		bphy_err(wiphy, "cmd (%d) , error (%d)\n", cmd, err);
		return err;
	}
	return err;
}