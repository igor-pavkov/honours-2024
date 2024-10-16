static av_always_inline void snow_interleave_line_footer(int * i, IDWTELEM * low, IDWTELEM * high){
    for (; (*i)>=0; (*i)-=2){
        low[(*i)+1] = high[(*i)>>1];
        low[*i] = low[(*i)>>1];
    }
}