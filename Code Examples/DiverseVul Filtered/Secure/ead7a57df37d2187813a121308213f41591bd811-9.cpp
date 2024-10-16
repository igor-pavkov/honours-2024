static void ssd0323_invalidate_display(void * opaque)
{
    ssd0323_state *s = (ssd0323_state *)opaque;
    s->redraw = 1;
}