static void pj_scan_syntax_err(pj_scanner *scanner)
{
    (*scanner->callback)(scanner);
}