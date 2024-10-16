static double labdiff(unsigned int rgb) {
    unsigned int r, g, b;
    const double L1 = 53.192777691077211f, A1 = 0.0031420942181448197f, B1 = -0.0062075877844014471f;
    double L2, A2, B2;

    r = (rgb>>16) & 0xff;
    g = (rgb>>8) & 0xff;
    b = rgb & 0xff;

    lab(r, g, b, &L2, &A2, &B2);

    return sqrt(pow(L1 - L2, 2.0f) + pow(A1 - A2, 2.0f) + pow(B1 - B2, 2.0f));
}