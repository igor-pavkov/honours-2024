void RuleMatchCandidateTxArrayInit(DetectEngineThreadCtx *det_ctx, uint32_t size)
{
    DEBUG_VALIDATE_BUG_ON(det_ctx->tx_candidates);
    det_ctx->tx_candidates = SCCalloc(size, sizeof(RuleMatchCandidateTx));
    if (det_ctx->tx_candidates == NULL) {
        FatalError(SC_ERR_MEM_ALLOC, "failed to allocate %"PRIu64" bytes",
                (uint64_t)(size * sizeof(RuleMatchCandidateTx)));
    }
    det_ctx->tx_candidates_size = size;
    SCLogDebug("array initialized to %u elements (%"PRIu64" bytes)",
            size, (uint64_t)(size * sizeof(RuleMatchCandidateTx)));
}