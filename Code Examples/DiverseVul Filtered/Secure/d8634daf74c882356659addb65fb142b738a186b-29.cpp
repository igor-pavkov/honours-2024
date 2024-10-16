void RuleMatchCandidateTxArrayFree(DetectEngineThreadCtx *det_ctx)
{
    SCFree(det_ctx->tx_candidates);
    det_ctx->tx_candidates_size = 0;
}