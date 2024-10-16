static void rfx_profiler_free(RFX_CONTEXT* context)
{
	PROFILER_FREE(context->priv->prof_rfx_decode_rgb)
	PROFILER_FREE(context->priv->prof_rfx_decode_component)
	PROFILER_FREE(context->priv->prof_rfx_rlgr_decode)
	PROFILER_FREE(context->priv->prof_rfx_differential_decode)
	PROFILER_FREE(context->priv->prof_rfx_quantization_decode)
	PROFILER_FREE(context->priv->prof_rfx_dwt_2d_decode)
	PROFILER_FREE(context->priv->prof_rfx_ycbcr_to_rgb)
	PROFILER_FREE(context->priv->prof_rfx_encode_rgb)
	PROFILER_FREE(context->priv->prof_rfx_encode_component)
	PROFILER_FREE(context->priv->prof_rfx_rlgr_encode)
	PROFILER_FREE(context->priv->prof_rfx_differential_encode)
	PROFILER_FREE(context->priv->prof_rfx_quantization_encode)
	PROFILER_FREE(context->priv->prof_rfx_dwt_2d_encode)
	PROFILER_FREE(context->priv->prof_rfx_rgb_to_ycbcr)
	PROFILER_FREE(context->priv->prof_rfx_encode_format_rgb)
}