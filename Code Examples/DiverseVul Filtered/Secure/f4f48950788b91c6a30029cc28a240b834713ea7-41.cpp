static int overlap(Segment *s1, Segment *s2)
{
    if (s1->a >= s2->b || s2->a >= s1->b ||
        s1->ha >= s2->hb || s2->ha >= s1->hb)
        return 0;
    return 1;
}