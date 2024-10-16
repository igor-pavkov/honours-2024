struct cfg80211_ops *brcmf_cfg80211_get_ops(struct brcmf_mp_device *settings)
{
	struct cfg80211_ops *ops;

	ops = kmemdup(&brcmf_cfg80211_ops, sizeof(brcmf_cfg80211_ops),
		       GFP_KERNEL);

	if (ops && settings->roamoff)
		ops->update_connect_params = NULL;

	return ops;
}