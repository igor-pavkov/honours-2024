static s32 brcmf_configure_opensecurity(struct brcmf_if *ifp)
{
	struct wiphy *wiphy = ifp->drvr->wiphy;
	s32 err;
	s32 wpa_val;

	/* set auth */
	err = brcmf_fil_bsscfg_int_set(ifp, "auth", 0);
	if (err < 0) {
		bphy_err(wiphy, "auth error %d\n", err);
		return err;
	}
	/* set wsec */
	err = brcmf_fil_bsscfg_int_set(ifp, "wsec", 0);
	if (err < 0) {
		bphy_err(wiphy, "wsec error %d\n", err);
		return err;
	}
	/* set upper-layer auth */
	if (brcmf_is_ibssmode(ifp->vif))
		wpa_val = WPA_AUTH_NONE;
	else
		wpa_val = WPA_AUTH_DISABLED;
	err = brcmf_fil_bsscfg_int_set(ifp, "wpa_auth", wpa_val);
	if (err < 0) {
		bphy_err(wiphy, "wpa_auth error %d\n", err);
		return err;
	}

	return 0;
}