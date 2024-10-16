static void free_macros(void)
{
    free_smacro_table(&smacros);
    free_mmacro_table(&mmacros);
}