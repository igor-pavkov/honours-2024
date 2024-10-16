static ossl_inline int number_of_digits(int bitsize, int digit_size)
{
    return (bitsize + digit_size - 1) / digit_size;
}