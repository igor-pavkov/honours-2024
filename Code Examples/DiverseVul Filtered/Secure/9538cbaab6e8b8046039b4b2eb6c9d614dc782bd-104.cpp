static int b43_op_set_key(struct ieee80211_hw *hw, enum set_key_cmd cmd,
			  struct ieee80211_vif *vif, struct ieee80211_sta *sta,
			  struct ieee80211_key_conf *key)
{
	struct b43_wl *wl = hw_to_b43_wl(hw);
	struct b43_wldev *dev;
	u8 algorithm;
	u8 index;
	int err;
	static const u8 bcast_addr[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	if (modparam_nohwcrypt)
		return -ENOSPC; /* User disabled HW-crypto */

	if ((vif->type == NL80211_IFTYPE_ADHOC ||
	     vif->type == NL80211_IFTYPE_MESH_POINT) &&
	    (key->cipher == WLAN_CIPHER_SUITE_TKIP ||
	     key->cipher == WLAN_CIPHER_SUITE_CCMP) &&
	    !(key->flags & IEEE80211_KEY_FLAG_PAIRWISE)) {
		/*
		 * For now, disable hw crypto for the RSN IBSS group keys. This
		 * could be optimized in the future, but until that gets
		 * implemented, use of software crypto for group addressed
		 * frames is a acceptable to allow RSN IBSS to be used.
		 */
		return -EOPNOTSUPP;
	}

	mutex_lock(&wl->mutex);

	dev = wl->current_dev;
	err = -ENODEV;
	if (!dev || b43_status(dev) < B43_STAT_INITIALIZED)
		goto out_unlock;

	if (dev->fw.pcm_request_failed || !dev->hwcrypto_enabled) {
		/* We don't have firmware for the crypto engine.
		 * Must use software-crypto. */
		err = -EOPNOTSUPP;
		goto out_unlock;
	}

	err = -EINVAL;
	switch (key->cipher) {
	case WLAN_CIPHER_SUITE_WEP40:
		algorithm = B43_SEC_ALGO_WEP40;
		break;
	case WLAN_CIPHER_SUITE_WEP104:
		algorithm = B43_SEC_ALGO_WEP104;
		break;
	case WLAN_CIPHER_SUITE_TKIP:
		algorithm = B43_SEC_ALGO_TKIP;
		break;
	case WLAN_CIPHER_SUITE_CCMP:
		algorithm = B43_SEC_ALGO_AES;
		break;
	default:
		B43_WARN_ON(1);
		goto out_unlock;
	}
	index = (u8) (key->keyidx);
	if (index > 3)
		goto out_unlock;

	switch (cmd) {
	case SET_KEY:
		if (algorithm == B43_SEC_ALGO_TKIP &&
		    (!(key->flags & IEEE80211_KEY_FLAG_PAIRWISE) ||
		    !modparam_hwtkip)) {
			/* We support only pairwise key */
			err = -EOPNOTSUPP;
			goto out_unlock;
		}

		if (key->flags & IEEE80211_KEY_FLAG_PAIRWISE) {
			if (WARN_ON(!sta)) {
				err = -EOPNOTSUPP;
				goto out_unlock;
			}
			/* Pairwise key with an assigned MAC address. */
			err = b43_key_write(dev, -1, algorithm,
					    key->key, key->keylen,
					    sta->addr, key);
		} else {
			/* Group key */
			err = b43_key_write(dev, index, algorithm,
					    key->key, key->keylen, NULL, key);
		}
		if (err)
			goto out_unlock;

		if (algorithm == B43_SEC_ALGO_WEP40 ||
		    algorithm == B43_SEC_ALGO_WEP104) {
			b43_hf_write(dev, b43_hf_read(dev) | B43_HF_USEDEFKEYS);
		} else {
			b43_hf_write(dev,
				     b43_hf_read(dev) & ~B43_HF_USEDEFKEYS);
		}
		key->flags |= IEEE80211_KEY_FLAG_GENERATE_IV;
		if (algorithm == B43_SEC_ALGO_TKIP)
			key->flags |= IEEE80211_KEY_FLAG_GENERATE_MMIC;
		break;
	case DISABLE_KEY: {
		err = b43_key_clear(dev, key->hw_key_idx);
		if (err)
			goto out_unlock;
		break;
	}
	default:
		B43_WARN_ON(1);
	}

out_unlock:
	if (!err) {
		b43dbg(wl, "%s hardware based encryption for keyidx: %d, "
		       "mac: %pM\n",
		       cmd == SET_KEY ? "Using" : "Disabling", key->keyidx,
		       sta ? sta->addr : bcast_addr);
		b43_dump_keymemory(dev);
	}
	mutex_unlock(&wl->mutex);

	return err;
}