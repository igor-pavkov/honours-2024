static void lab(double r, double g, double b, double *L, double *A, double *B) {
    double x, y, z;
    r /= 255.0f;
    g /= 255.0f;
    b /= 255.0f;

    if (r > 0.04045f) r = pow(((r + 0.055f) / 1.055f), 2.4f);
    else r /= 12.92f;
    if (g > 0.04045f) g = pow(((g + 0.055f) / 1.055f), 2.4f);
    else g /= 12.92f;
    if (b > 0.04045f) b = pow(((b + 0.055f) / 1.055f), 2.4f);
    else b /= 12.92f;

    r *= 100.0f;
    g *= 100.0f;
    b *= 100.0f;

    x = r * 0.4124f + g * 0.3576f + b * 0.1805f;
    y = r * 0.2126f + g * 0.7152f + b * 0.0722f;
    z = r * 0.0193f + g * 0.1192f + b * 0.9505f;

    x /= 95.047f;
    y /= 100.000f;
    z /= 108.883f;

    if (x > 0.008856f) x = pow(x, 1.0f/3.0f);
    else x = (7.787f * x) + (16.0f / 116.0f);
    if (y > 0.008856f) y = pow(y, (1.0f/3.0f));
    else y = (7.787f * y) + (16.0f / 116.0f);
    if (z > 0.008856f) z = pow(z, (1.0f/3.0f));
    else z = (7.787f * z) + (16.0f / 116.0f);

    *L = (116.0f * y) - 16.0f;
    *A = 500.0f * (x - y);
    *B = 200.0f * (y - z);
}