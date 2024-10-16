static unsigned recover_key(unsigned char sample[4], unsigned expected_size)
{
    unsigned char plaintext[8] = { 'S', 'B' };

    put_v(plaintext+2, expected_size);

    return AV_RL32(sample) ^ AV_RL32(plaintext);
}