  edge_clone_summary_t (symbol_table *symtab):
    call_summary <edge_clone_summary *> (symtab)
    {
      m_initialize_when_cloning = true;
    }