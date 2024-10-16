static void apply_independent_coupling(AACContext *ac,
                                       SingleChannelElement *target,
                                       ChannelElement *cce, int index)
{
    int i;
    const float gain = cce->coup.gain[index][0];
    const float *src = cce->ch[0].ret;
    float *dest = target->ret;
    const int len = 1024 << (ac->oc[1].m4ac.sbr == 1);

    for (i = 0; i < len; i++)
        dest[i] += gain * src[i];
}