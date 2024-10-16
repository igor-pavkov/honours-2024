static void naldmx_bs_log(void *udta, const char *field_name, u32 nb_bits, u64 field_val, s32 idx1, s32 idx2, s32 idx3)
{
	GF_NALUDmxCtx *ctx = (GF_NALUDmxCtx *) udta;
	GF_LOG(GF_LOG_DEBUG, GF_LOG_MEDIA, (" %s", field_name));
	if (idx1>=0) {
		GF_LOG(GF_LOG_DEBUG, GF_LOG_MEDIA, ("_%d", idx1));
		if (idx2>=0) {
			GF_LOG(GF_LOG_DEBUG, GF_LOG_MEDIA, ("_%d", idx2));
			if (idx3>=0) {
				GF_LOG(GF_LOG_DEBUG, GF_LOG_MEDIA, ("_%d", idx3));
			}
		}
	}
	GF_LOG(GF_LOG_DEBUG, GF_LOG_MEDIA, ("=\""LLD, field_val));
	if ((ctx->bsdbg==2) && ((s32) nb_bits > 1) )
		GF_LOG(GF_LOG_DEBUG, GF_LOG_MEDIA, ("(%u)", nb_bits));

	GF_LOG(GF_LOG_DEBUG, GF_LOG_MEDIA, ("\" "));
}