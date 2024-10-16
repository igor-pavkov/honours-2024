static int b43_key_write(struct b43_wldev *dev,
			 int index, u8 algorithm,
			 const u8 *key, size_t key_len,
			 const u8 *mac_addr,
			 struct ieee80211_key_conf *keyconf)
{
	int i;
	int pairwise_keys_start;

	/* For ALG_TKIP the key is encoded as a 256-bit (32 byte) data block:
	 * 	- Temporal Encryption Key (128 bits)
	 * 	- Temporal Authenticator Tx MIC Key (64 bits)
	 * 	- Temporal Authenticator Rx MIC Key (64 bits)
	 *
	 * 	Hardware only store TEK
	 */
	if (algorithm == B43_SEC_ALGO_TKIP && key_len == 32)
		key_len = 16;
	if (key_len > B43_SEC_KEYSIZE)
		return -EINVAL;
	for (i = 0; i < ARRAY_SIZE(dev->key); i++) {
		/* Check that we don't already have this key. */
		B43_WARN_ON(dev->key[i].keyconf == keyconf);
	}
	if (index < 0) {
		/* Pairwise key. Get an empty slot for the key. */
		if (b43_new_kidx_api(dev))
			pairwise_keys_start = B43_NR_GROUP_KEYS;
		else
			pairwise_keys_start = B43_NR_GROUP_KEYS * 2;
		for (i = pairwise_keys_start;
		     i < pairwise_keys_start + B43_NR_PAIRWISE_KEYS;
		     i++) {
			B43_WARN_ON(i >= ARRAY_SIZE(dev->key));
			if (!dev->key[i].keyconf) {
				/* found empty */
				index = i;
				break;
			}
		}
		if (index < 0) {
			b43warn(dev->wl, "Out of hardware key memory\n");
			return -ENOSPC;
		}
	} else
		B43_WARN_ON(index > 3);

	do_key_write(dev, index, algorithm, key, key_len, mac_addr);
	if ((index <= 3) && !b43_new_kidx_api(dev)) {
		/* Default RX key */
		B43_WARN_ON(mac_addr);
		do_key_write(dev, index + 4, algorithm, key, key_len, NULL);
	}
	keyconf->hw_key_idx = index;
	dev->key[index].keyconf = keyconf;

	return 0;
}