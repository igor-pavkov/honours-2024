static inline void DetectRunPrefilterPkt(
    ThreadVars *tv,
    DetectEngineCtx *de_ctx,
    DetectEngineThreadCtx *det_ctx,
    Packet *p,
    DetectRunScratchpad *scratch
)
{
    DetectPrefilterSetNonPrefilterList(p, det_ctx, scratch);

    /* create our prefilter mask */
    PacketCreateMask(p, &scratch->pkt_mask, scratch->alproto, scratch->app_decoder_events);

    /* build and prefilter non_pf list against the mask of the packet */
    PACKET_PROFILING_DETECT_START(p, PROF_DETECT_NONMPMLIST);
    det_ctx->non_pf_id_cnt = 0;
    if (likely(det_ctx->non_pf_store_cnt > 0)) {
        DetectPrefilterBuildNonPrefilterList(det_ctx, scratch->pkt_mask, scratch->alproto);
    }
    PACKET_PROFILING_DETECT_END(p, PROF_DETECT_NONMPMLIST);

    /* run the prefilter engines */
    Prefilter(det_ctx, scratch->sgh, p, scratch->flow_flags);
    /* create match list if we have non-pf and/or pf */
    if (det_ctx->non_pf_store_cnt || det_ctx->pmq.rule_id_array_cnt) {
        PACKET_PROFILING_DETECT_START(p, PROF_DETECT_PF_SORT2);
        DetectPrefilterMergeSort(de_ctx, det_ctx);
        PACKET_PROFILING_DETECT_END(p, PROF_DETECT_PF_SORT2);
    }

#ifdef PROFILING
    if (tv) {
        StatsAddUI64(tv, det_ctx->counter_mpm_list,
                             (uint64_t)det_ctx->pmq.rule_id_array_cnt);
        StatsAddUI64(tv, det_ctx->counter_nonmpm_list,
                             (uint64_t)det_ctx->non_pf_store_cnt);
        /* non mpm sigs after mask prefilter */
        StatsAddUI64(tv, det_ctx->counter_fnonmpm_list,
                             (uint64_t)det_ctx->non_pf_id_cnt);
    }
#endif
}