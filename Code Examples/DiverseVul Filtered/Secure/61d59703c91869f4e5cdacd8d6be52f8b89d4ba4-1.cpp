static av_always_inline void snow_interleave_line_header(int * i, int width, IDWTELEM * low, IDWTELEM * high){
    (*i) = (width) - 2;

    if (width & 1){
        low[(*i)+1] = low[((*i)+1)>>1];
        (*i)--;
    }
}