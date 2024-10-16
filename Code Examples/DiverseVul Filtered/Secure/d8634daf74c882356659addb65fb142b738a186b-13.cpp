static int RuleMatchCandidateTxArrayExpand(DetectEngineThreadCtx *det_ctx, const uint32_t needed)
{
    const uint32_t old_size = det_ctx->tx_candidates_size;
    uint32_t new_size = needed;
    void *ptmp = SCRealloc(det_ctx->tx_candidates, (new_size * sizeof(RuleMatchCandidateTx)));
    if (ptmp == NULL) {
        FatalError(SC_ERR_MEM_ALLOC, "failed to expand to %"PRIu64" bytes",
                (uint64_t)(new_size * sizeof(RuleMatchCandidateTx)));
        // TODO can this be handled more gracefully?
    }
    det_ctx->tx_candidates = ptmp;
    det_ctx->tx_candidates_size = new_size;
    SCLogDebug("array expanded from %u to %u elements (%"PRIu64" bytes -> %"PRIu64" bytes)",
            old_size, new_size, (uint64_t)(old_size * sizeof(RuleMatchCandidateTx)),
            (uint64_t)(new_size * sizeof(RuleMatchCandidateTx))); (void)old_size;
    return 1;
}