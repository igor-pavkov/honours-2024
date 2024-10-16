int main()
{
    gdImagePtr im;

    im = gdImageCreate(64970, 65111);
    gdTestAssert(im == NULL);

    return gdNumFailures();
}