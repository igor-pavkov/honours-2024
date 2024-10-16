static int fit_segment(Segment *s, Segment *fixed, int *cnt, int dir)
{
    int i;
    int shift = 0;

    if (dir == 1)               // move down
        for (i = 0; i < *cnt; ++i) {
            if (s->b + shift <= fixed[i].a || s->a + shift >= fixed[i].b ||
                s->hb <= fixed[i].ha || s->ha >= fixed[i].hb)
                continue;
            shift = fixed[i].b - s->a;
    } else                      // dir == -1, move up
        for (i = *cnt - 1; i >= 0; --i) {
            if (s->b + shift <= fixed[i].a || s->a + shift >= fixed[i].b ||
                s->hb <= fixed[i].ha || s->ha >= fixed[i].hb)
                continue;
            shift = fixed[i].a - s->b;
        }

    fixed[*cnt].a = s->a + shift;
    fixed[*cnt].b = s->b + shift;
    fixed[*cnt].ha = s->ha;
    fixed[*cnt].hb = s->hb;
    (*cnt)++;
    qsort(fixed, *cnt, sizeof(Segment), cmp_segment);

    return shift;
}