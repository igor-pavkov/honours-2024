static void do_key_write(struct b43_wldev *dev,
			 u8 index, u8 algorithm,
			 const u8 *key, size_t key_len, const u8 *mac_addr)
{
	u8 buf[B43_SEC_KEYSIZE] = { 0, };
	u8 pairwise_keys_start = B43_NR_GROUP_KEYS * 2;

	if (b43_new_kidx_api(dev))
		pairwise_keys_start = B43_NR_GROUP_KEYS;

	B43_WARN_ON(index >= ARRAY_SIZE(dev->key));
	B43_WARN_ON(key_len > B43_SEC_KEYSIZE);

	if (index >= pairwise_keys_start)
		keymac_write(dev, index, NULL);	/* First zero out mac. */
	if (algorithm == B43_SEC_ALGO_TKIP) {
		/*
		 * We should provide an initial iv32, phase1key pair.
		 * We could start with iv32=0 and compute the corresponding
		 * phase1key, but this means calling ieee80211_get_tkip_key
		 * with a fake skb (or export other tkip function).
		 * Because we are lazy we hope iv32 won't start with
		 * 0xffffffff and let's b43_op_update_tkip_key provide a
		 * correct pair.
		 */
		rx_tkip_phase1_write(dev, index, 0xffffffff, (u16*)buf);
	} else if (index >= pairwise_keys_start) /* clear it */
		rx_tkip_phase1_write(dev, index, 0, NULL);
	if (key)
		memcpy(buf, key, key_len);
	key_write(dev, index, algorithm, buf);
	if (index >= pairwise_keys_start)
		keymac_write(dev, index, mac_addr);

	dev->key[index].algorithm = algorithm;
}