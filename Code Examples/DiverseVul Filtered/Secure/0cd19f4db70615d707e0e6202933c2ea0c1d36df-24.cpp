static void vp9_segmentation_params(GF_BitStream *bs)
{
	Bool segmentation_enabled = gf_bs_read_int_log(bs, 1, "segmentation_enabled");
	if (segmentation_enabled == 1) {
		int i;
		Bool segmentation_update_map = gf_bs_read_int_log(bs, 1, "segmentation_update_map");
		if (segmentation_update_map) {
			for (i = 0; i < 7; i++) {
				vp9_read_prob(bs);
			}
			Bool segmentation_temporal_update = gf_bs_read_int_log(bs, 1, "segmentation_temporal_update");
			for (i = 0; i < 3; i++) {
				if (segmentation_temporal_update) {
					vp9_read_prob(bs);
				}
			}
		}
		Bool segmentation_update_data = gf_bs_read_int_log(bs, 1, "segmentation_update_data");
		if (segmentation_update_data == 1) {
			/*segmentation_abs_or_delta_update =*/ gf_bs_read_int_log(bs, 1, "segmentation_abs_or_delta_update");
			for (i = 0; i < VP9_MAX_SEGMENTS; i++) {
				int j;
				for (j = 0; j < VP9_SEG_LVL_MAX; j++) {
					/*feature_value = 0*/
					Bool feature_enabled = gf_bs_read_int_log(bs, 1, "feature_enabled");
					/*FeatureEnabled[i][j] = feature_enabled*/
					if (feature_enabled) {
						int bits_to_read = segmentation_feature_bits[j];
						/*feature_value =*/ gf_bs_read_int_log(bs, bits_to_read, "feature_value");
						if (segmentation_feature_signed[j] == 1) {
							/*Bool feature_sign = */gf_bs_read_int_log(bs, 1, "feature_sign");
							/*if (feature_sign == 1)
								feature_value *= -1*/
						}
					}
					/*FeatureData[i][j] = feature_value*/
				}
			}
		}
	}
}