static int is_open_key_sample(const MOVStreamContext *sc, int sample)
{
    // TODO: a bisect search would scale much better
    for (int i = 0; i < sc->open_key_samples_count; i++) {
        const int oks = sc->open_key_samples[i];
        if (oks == sample)
            return 1;
        if (oks > sample) /* list is monotically increasing so we can stop early */
            break;
    }
    return 0;
}