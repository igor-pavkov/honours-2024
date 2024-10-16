static int viv_probe(const AVProbeData *p)
{
    if (memcmp(p->buf, "vividas03", 9))
        return 0;

    return AVPROBE_SCORE_MAX;
}