static int aud_to_ossfmt (AudioFormat fmt, int endianness)
{
    switch (fmt) {
    case AUDIO_FORMAT_S8:
        return AFMT_S8;

    case AUDIO_FORMAT_U8:
        return AFMT_U8;

    case AUDIO_FORMAT_S16:
        if (endianness) {
            return AFMT_S16_BE;
        }
        else {
            return AFMT_S16_LE;
        }

    case AUDIO_FORMAT_U16:
        if (endianness) {
            return AFMT_U16_BE;
        }
        else {
            return AFMT_U16_LE;
        }

    default:
        dolog ("Internal logic error: Bad audio format %d\n", fmt);
#ifdef DEBUG_AUDIO
        abort ();
#endif
        return AFMT_U8;
    }
}