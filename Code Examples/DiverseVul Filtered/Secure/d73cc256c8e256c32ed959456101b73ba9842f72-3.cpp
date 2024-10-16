static void message(BIO *out, char *m)
{
    fprintf(stderr, "test %s\n", m);
    BIO_puts(out, "print \"test ");
    BIO_puts(out, m);
    BIO_puts(out, "\\n\"\n");
}