static int cmp_segment(const void *p1, const void *p2)
{
    return ((Segment *) p1)->a - ((Segment *) p2)->a;
}