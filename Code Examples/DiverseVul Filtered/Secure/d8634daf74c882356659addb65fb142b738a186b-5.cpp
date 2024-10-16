static void DetectRunPostMatch(ThreadVars *tv,
                               DetectEngineThreadCtx *det_ctx, Packet *p,
                               const Signature *s)
{
    /* run the packet match functions */
    const SigMatchData *smd = s->sm_arrays[DETECT_SM_LIST_POSTMATCH];
    if (smd != NULL) {
        KEYWORD_PROFILING_SET_LIST(det_ctx, DETECT_SM_LIST_POSTMATCH);

        SCLogDebug("running match functions, sm %p", smd);

        while (1) {
            KEYWORD_PROFILING_START;
            (void)sigmatch_table[smd->type].Match(tv, det_ctx, p, s, smd->ctx);
            KEYWORD_PROFILING_END(det_ctx, smd->type, 1);
            if (smd->is_last)
                break;
            smd++;
        }
    }

    DetectReplaceExecute(p, det_ctx);

    if (s->flags & SIG_FLAG_FILESTORE)
        DetectFilestorePostMatch(tv, det_ctx, p, s);

    return;
}