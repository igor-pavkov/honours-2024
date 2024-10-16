static void show_version(void)
{
    printf("NASM version %s compiled on %s%s\n",
           nasm_version, nasm_date, nasm_compile_options);
    exit(0);
}