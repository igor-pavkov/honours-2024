static inline bool RuleMatchCandidateTxArrayHasSpace(const DetectEngineThreadCtx *det_ctx,
        const uint32_t need)
{
    if (det_ctx->tx_candidates_size >= need)
        return 1;
    return 0;
}