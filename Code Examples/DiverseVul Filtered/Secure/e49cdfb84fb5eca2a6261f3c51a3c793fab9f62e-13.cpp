static void ok_png_decode_filter(uint8_t * RESTRICT curr, const uint8_t * RESTRICT prev,
                                 size_t length, int filter, uint8_t bpp) {
    switch (filter) {
        case OK_PNG_FILTER_NONE:
            // Do nothing
            break;
        case OK_PNG_FILTER_SUB: {
            // Input = Sub
            // Raw(x) = Sub(x) + Raw(x-bpp)
            // For all x < 0, assume Raw(x) = 0.
            for (size_t i = bpp; i < length; i++) {
                curr[i] = curr[i] + curr[i - bpp];
            }
            break;
        }
        case OK_PNG_FILTER_UP: {
            // Input = Up
            // Raw(x) = Up(x) + Prior(x)
            for (size_t i = 0; i < length; i++) {
                curr[i] = curr[i] + prev[i];
            }
            break;
        }
        case OK_PNG_FILTER_AVG: {
            // Input = Average
            // Raw(x) = Average(x) + floor((Raw(x-bpp)+Prior(x))/2)
            for (size_t i = 0; i < bpp; i++) {
                curr[i] = curr[i] + (prev[i] >> 1);
            }
            for (size_t j = bpp; j < length; j++) {
                curr[j] = curr[j] + ((curr[j - bpp] + prev[j]) >> 1);
            }
            break;
        }
        case OK_PNG_FILTER_PAETH: {
            // Input = Paeth
            // Raw(x) = Paeth(x) + PaethPredictor(Raw(x-bpp), Prior(x), Prior(x-bpp))
            for (size_t i = 0; i < bpp; i++) {
                curr[i] += prev[i];
            }
            for (size_t j = bpp; j < length; j++) {
                curr[j] += ok_png_paeth_predictor(curr[j - bpp], prev[j], prev[j - bpp]);
            }
            break;
        }
    }
}