void ass_shaper_set_kerning(ASS_Shaper *shaper, int kern)
{
#ifdef CONFIG_HARFBUZZ
    shaper->features[KERN].value = !!kern;
#endif
}