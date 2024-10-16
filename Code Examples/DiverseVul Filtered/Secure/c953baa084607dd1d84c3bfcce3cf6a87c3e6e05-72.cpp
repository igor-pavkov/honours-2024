static inline int mov_stsc_index_valid(unsigned int index, unsigned int count)
{
    return index < count - 1;
}