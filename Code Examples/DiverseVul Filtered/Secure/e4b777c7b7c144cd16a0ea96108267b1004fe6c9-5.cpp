static gif_result gif_error_from_lzw(lzw_result l_res)
{
        static const gif_result g_res[] = {
                [LZW_OK]        = GIF_OK,
                [LZW_OK_EOD]    = GIF_END_OF_FRAME,
                [LZW_NO_MEM]    = GIF_INSUFFICIENT_MEMORY,
                [LZW_NO_DATA]   = GIF_INSUFFICIENT_FRAME_DATA,
                [LZW_EOI_CODE]  = GIF_FRAME_DATA_ERROR,
                [LZW_BAD_ICODE] = GIF_FRAME_DATA_ERROR,
                [LZW_BAD_CODE]  = GIF_FRAME_DATA_ERROR,
        };
        return g_res[l_res];
}