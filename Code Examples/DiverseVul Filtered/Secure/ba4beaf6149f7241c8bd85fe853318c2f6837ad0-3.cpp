static av_always_inline int filter_3800(APEPredictor *p,
                                        const int decoded, const int filter,
                                        const int delayA,  const int delayB,
                                        const int start,   const int shift)
{
    int32_t predictionA, predictionB, sign;
    int32_t d0, d1, d2, d3, d4;

    p->buf[delayA] = p->lastA[filter];
    p->buf[delayB] = p->filterB[filter];
    if (p->sample_pos < start) {
        predictionA = decoded + p->filterA[filter];
        p->lastA[filter]   = decoded;
        p->filterB[filter] = decoded;
        p->filterA[filter] = predictionA;
        return predictionA;
    }
    d2 =  p->buf[delayA];
    d1 = (p->buf[delayA] - p->buf[delayA - 1]) << 1;
    d0 =  p->buf[delayA] + ((p->buf[delayA - 2] - p->buf[delayA - 1]) << 3);
    d3 =  p->buf[delayB] * 2 - p->buf[delayB - 1];
    d4 =  p->buf[delayB];

    predictionA = d0 * p->coeffsA[filter][0] +
                  d1 * p->coeffsA[filter][1] +
                  d2 * p->coeffsA[filter][2];

    sign = APESIGN(decoded);
    p->coeffsA[filter][0] += (((d0 >> 30) & 2) - 1) * sign;
    p->coeffsA[filter][1] += (((d1 >> 28) & 8) - 4) * sign;
    p->coeffsA[filter][2] += (((d2 >> 28) & 8) - 4) * sign;

    predictionB = d3 * p->coeffsB[filter][0] -
                  d4 * p->coeffsB[filter][1];
    p->lastA[filter] = decoded + (predictionA >> 11);
    sign = APESIGN(p->lastA[filter]);
    p->coeffsB[filter][0] += (((d3 >> 29) & 4) - 2) * sign;
    p->coeffsB[filter][1] -= (((d4 >> 30) & 2) - 1) * sign;

    p->filterB[filter] = p->lastA[filter] + (predictionB >> shift);
    p->filterA[filter] = p->filterB[filter] + ((p->filterA[filter] * 31) >> 5);

    return p->filterA[filter];
}