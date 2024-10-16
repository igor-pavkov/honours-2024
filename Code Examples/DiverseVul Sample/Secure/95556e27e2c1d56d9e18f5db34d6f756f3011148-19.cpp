static int is_cover_image(const AVStream *st)
{
    /* Eg. AV_DISPOSITION_ATTACHED_PIC | AV_DISPOSITION_TIMED_THUMBNAILS
     * is encoded as sparse video track */
    return st && st->disposition == AV_DISPOSITION_ATTACHED_PIC;
}