static inline int DetectRunInspectRulePacketMatches(
    ThreadVars *tv,
    DetectEngineThreadCtx *det_ctx,
    Packet *p,
    const Flow *f,
    const Signature *s)
{
    /* run the packet match functions */
    if (s->sm_arrays[DETECT_SM_LIST_MATCH] != NULL) {
        KEYWORD_PROFILING_SET_LIST(det_ctx, DETECT_SM_LIST_MATCH);
        SigMatchData *smd = s->sm_arrays[DETECT_SM_LIST_MATCH];

        SCLogDebug("running match functions, sm %p", smd);
        if (smd != NULL) {
            while (1) {
                KEYWORD_PROFILING_START;
                if (sigmatch_table[smd->type].Match(tv, det_ctx, p, s, smd->ctx) <= 0) {
                    KEYWORD_PROFILING_END(det_ctx, smd->type, 0);
                    SCLogDebug("no match");
                    return 0;
                }
                KEYWORD_PROFILING_END(det_ctx, smd->type, 1);
                if (smd->is_last) {
                    SCLogDebug("match and is_last");
                    break;
                }
                smd++;
            }
        }
    }
    return 1;
}