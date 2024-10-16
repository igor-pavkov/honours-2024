static av_always_inline void snow_horizontal_compose_liftS_lead_out(int i, IDWTELEM * dst, IDWTELEM * src, IDWTELEM * ref, int width, int w){
        for(; i<w; i++){
            dst[i] = src[i] + ((ref[i] + ref[(i+1)]+W_BO + 4 * src[i]) >> W_BS);
        }

        if(width&1){
            dst[w] = src[w] + ((2 * ref[w] + W_BO + 4 * src[w]) >> W_BS);
        }
}