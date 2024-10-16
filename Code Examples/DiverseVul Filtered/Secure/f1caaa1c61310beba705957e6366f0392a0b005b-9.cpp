static av_always_inline int dnxhd_switch_matrix(DNXHDEncContext *ctx, int i)
{
    const static uint8_t component[8]={0,0,1,2,0,0,1,2};
    return component[i];
}