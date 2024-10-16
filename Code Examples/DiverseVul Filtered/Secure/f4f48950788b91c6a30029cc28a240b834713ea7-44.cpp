void ass_frame_ref(ASS_Image *img)
{
    if (!img)
        return;
    ((ASS_ImagePriv *) img)->ref_count++;
}