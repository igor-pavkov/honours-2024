server_method* find_method(XMLRPC_SERVER server, const char* name) {
   server_method* sm;

   q_iter qi = Q_Iter_Head_F(&server->methodlist);

   while( qi ) {
      sm = Q_Iter_Get_F(qi);
      if(sm && !strcmp(sm->name, name)) {
         return sm;
      }
      qi = Q_Iter_Next_F(qi);
   }
   return NULL;
}