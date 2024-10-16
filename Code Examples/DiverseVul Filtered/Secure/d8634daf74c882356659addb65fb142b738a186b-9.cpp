void SigMatchSignatures(ThreadVars *th_v,
        DetectEngineCtx *de_ctx, DetectEngineThreadCtx *det_ctx,
        Packet *p)
{
    DetectRun(th_v, de_ctx, det_ctx, p);
}