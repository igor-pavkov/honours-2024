static void populate_default_features(void)
{
	int i;

	for (i = 0; passthrough_map[i].feature_bit != 0xff; i++) {
		if (avctp_supports_avc(passthrough_map[i].avc)) {
			uint8_t bit = passthrough_map[i].feature_bit;

			default_features[bit >> 3] |= (1 << (bit & 7));
		}
	}

	/* supports at least AVRCP 1.4 */
	default_features[7] |= (1 << 2);

	/* supports GetTotalNumberOfItems browsing command */
	default_features[8] |= (1 << 3);
}