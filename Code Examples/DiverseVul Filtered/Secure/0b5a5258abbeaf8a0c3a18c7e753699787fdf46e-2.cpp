  virtual Stored_program_creation_ctx *clone(MEM_ROOT *mem_root)
  {
    return new (mem_root)
               Event_creation_ctx(m_client_cs, m_connection_cl, m_db_cl);
  }