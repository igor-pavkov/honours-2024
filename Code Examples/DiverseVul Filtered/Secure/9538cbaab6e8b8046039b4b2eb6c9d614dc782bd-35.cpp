static void b43_dump_keymemory(struct b43_wldev *dev)
{
	unsigned int i, index, count, offset, pairwise_keys_start;
	u8 mac[ETH_ALEN];
	u16 algo;
	u32 rcmta0;
	u16 rcmta1;
	u64 hf;
	struct b43_key *key;

	if (!b43_debug(dev, B43_DBG_KEYS))
		return;

	hf = b43_hf_read(dev);
	b43dbg(dev->wl, "Hardware key memory dump:  USEDEFKEYS=%u\n",
	       !!(hf & B43_HF_USEDEFKEYS));
	if (b43_new_kidx_api(dev)) {
		pairwise_keys_start = B43_NR_GROUP_KEYS;
		count = B43_NR_GROUP_KEYS + B43_NR_PAIRWISE_KEYS;
	} else {
		pairwise_keys_start = B43_NR_GROUP_KEYS * 2;
		count = B43_NR_GROUP_KEYS * 2 + B43_NR_PAIRWISE_KEYS;
	}
	for (index = 0; index < count; index++) {
		key = &(dev->key[index]);
		printk(KERN_DEBUG "Key slot %02u: %s",
		       index, (key->keyconf == NULL) ? " " : "*");
		offset = dev->ktp + (index * B43_SEC_KEYSIZE);
		for (i = 0; i < B43_SEC_KEYSIZE; i += 2) {
			u16 tmp = b43_shm_read16(dev, B43_SHM_SHARED, offset + i);
			printk("%02X%02X", (tmp & 0xFF), ((tmp >> 8) & 0xFF));
		}

		algo = b43_shm_read16(dev, B43_SHM_SHARED,
				      B43_SHM_SH_KEYIDXBLOCK + (index * 2));
		printk("   Algo: %04X/%02X", algo, key->algorithm);

		if (index >= pairwise_keys_start) {
			if (key->algorithm == B43_SEC_ALGO_TKIP) {
				printk("   TKIP: ");
				offset = B43_SHM_SH_TKIPTSCTTAK + (index - 4) * (10 + 4);
				for (i = 0; i < 14; i += 2) {
					u16 tmp = b43_shm_read16(dev, B43_SHM_SHARED, offset + i);
					printk("%02X%02X", (tmp & 0xFF), ((tmp >> 8) & 0xFF));
				}
			}
			rcmta0 = b43_shm_read32(dev, B43_SHM_RCMTA,
						((index - pairwise_keys_start) * 2) + 0);
			rcmta1 = b43_shm_read16(dev, B43_SHM_RCMTA,
						((index - pairwise_keys_start) * 2) + 1);
			*((__le32 *)(&mac[0])) = cpu_to_le32(rcmta0);
			*((__le16 *)(&mac[4])) = cpu_to_le16(rcmta1);
			printk("   MAC: %pM", mac);
		} else
			printk("   DEFAULT KEY");
		printk("\n");
	}
}