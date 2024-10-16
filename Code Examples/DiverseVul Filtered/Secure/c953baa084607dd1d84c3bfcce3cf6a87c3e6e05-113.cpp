static inline int64_t mov_get_stsc_samples(MOVStreamContext *sc, unsigned int index)
{
    int chunk_count;

    if (mov_stsc_index_valid(index, sc->stsc_count))
        chunk_count = sc->stsc_data[index + 1].first - sc->stsc_data[index].first;
    else {
        // Validation for stsc / stco  happens earlier in mov_read_stsc + mov_read_trak.
        av_assert0(sc->stsc_data[index].first <= sc->chunk_count);
        chunk_count = sc->chunk_count - (sc->stsc_data[index].first - 1);
    }

    return sc->stsc_data[index].count * (int64_t)chunk_count;
}