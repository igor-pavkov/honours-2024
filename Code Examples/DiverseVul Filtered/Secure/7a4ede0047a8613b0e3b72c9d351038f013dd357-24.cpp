static int oss_to_audfmt (int ossfmt, AudioFormat *fmt, int *endianness)
{
    switch (ossfmt) {
    case AFMT_S8:
        *endianness = 0;
        *fmt = AUDIO_FORMAT_S8;
        break;

    case AFMT_U8:
        *endianness = 0;
        *fmt = AUDIO_FORMAT_U8;
        break;

    case AFMT_S16_LE:
        *endianness = 0;
        *fmt = AUDIO_FORMAT_S16;
        break;

    case AFMT_U16_LE:
        *endianness = 0;
        *fmt = AUDIO_FORMAT_U16;
        break;

    case AFMT_S16_BE:
        *endianness = 1;
        *fmt = AUDIO_FORMAT_S16;
        break;

    case AFMT_U16_BE:
        *endianness = 1;
        *fmt = AUDIO_FORMAT_U16;
        break;

    default:
        dolog ("Unrecognized audio format %d\n", ossfmt);
        return -1;
    }

    return 0;
}