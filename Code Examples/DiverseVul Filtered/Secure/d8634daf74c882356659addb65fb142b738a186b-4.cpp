static inline void DetectPrefilterMergeSort(DetectEngineCtx *de_ctx,
                                            DetectEngineThreadCtx *det_ctx)
{
    SigIntId mpm, nonmpm;
    det_ctx->match_array_cnt = 0;
    SigIntId *mpm_ptr = det_ctx->pmq.rule_id_array;
    SigIntId *nonmpm_ptr = det_ctx->non_pf_id_array;
    uint32_t m_cnt = det_ctx->pmq.rule_id_array_cnt;
    uint32_t n_cnt = det_ctx->non_pf_id_cnt;
    SigIntId *final_ptr;
    uint32_t final_cnt;
    SigIntId id;
    SigIntId previous_id = (SigIntId)-1;
    Signature **sig_array = de_ctx->sig_array;
    Signature **match_array = det_ctx->match_array;
    Signature *s;

    SCLogDebug("PMQ rule id array count %d", det_ctx->pmq.rule_id_array_cnt);

    /* Load first values. */
    if (likely(m_cnt)) {
        mpm = *mpm_ptr;
    } else {
        /* mpm list is empty */
        final_ptr = nonmpm_ptr;
        final_cnt = n_cnt;
        goto final;
    }
    if (likely(n_cnt)) {
        nonmpm = *nonmpm_ptr;
    } else {
        /* non-mpm list is empty. */
        final_ptr = mpm_ptr;
        final_cnt = m_cnt;
        goto final;
    }
    while (1) {
        if (mpm < nonmpm) {
            /* Take from mpm list */
            id = mpm;

            s = sig_array[id];
            /* As the mpm list can contain duplicates, check for that here. */
            if (likely(id != previous_id)) {
                *match_array++ = s;
                previous_id = id;
            }
            if (unlikely(--m_cnt == 0)) {
                /* mpm list is now empty */
                final_ptr = nonmpm_ptr;
                final_cnt = n_cnt;
                goto final;
             }
             mpm_ptr++;
             mpm = *mpm_ptr;
         } else if (mpm > nonmpm) {
             id = nonmpm;

             s = sig_array[id];
             /* As the mpm list can contain duplicates, check for that here. */
             if (likely(id != previous_id)) {
                 *match_array++ = s;
                 previous_id = id;
             }
             if (unlikely(--n_cnt == 0)) {
                 final_ptr = mpm_ptr;
                 final_cnt = m_cnt;
                 goto final;
             }
             nonmpm_ptr++;
             nonmpm = *nonmpm_ptr;

        } else { /* implied mpm == nonmpm */
            /* special case: if on both lists, it's a negated mpm pattern */

            /* mpm list may have dups, so skip past them here */
            while (--m_cnt != 0) {
                mpm_ptr++;
                mpm = *mpm_ptr;
                if (mpm != nonmpm)
                    break;
            }
            /* if mpm is done, update nonmpm_ptrs and jump to final */
            if (unlikely(m_cnt == 0)) {
                n_cnt--;

                /* mpm list is now empty */
                final_ptr = ++nonmpm_ptr;
                final_cnt = n_cnt;
                goto final;
            }
            /* otherwise, if nonmpm is done jump to final for mpm
             * mpm ptrs alrady updated */
            if (unlikely(--n_cnt == 0)) {
                final_ptr = mpm_ptr;
                final_cnt = m_cnt;
                goto final;
            }

            /* not at end of the lists, update nonmpm. Mpm already
             * updated in while loop above. */
            nonmpm_ptr++;
            nonmpm = *nonmpm_ptr;
        }
    }

 final: /* Only one list remaining. Just walk that list. */

    while (final_cnt-- > 0) {
        id = *final_ptr++;
        s = sig_array[id];

        /* As the mpm list can contain duplicates, check for that here. */
        if (likely(id != previous_id)) {
            *match_array++ = s;
            previous_id = id;
        }
    }

    det_ctx->match_array_cnt = match_array - det_ctx->match_array;

    BUG_ON((det_ctx->pmq.rule_id_array_cnt + det_ctx->non_pf_id_cnt) < det_ctx->match_array_cnt);
}