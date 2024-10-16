static void test_bug5194()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND *my_bind;
  char *query;
  char *param_str;
  int param_str_length;
  const char *stmt_text;
  int rc;
  float float_array[250] =
  {
    0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
    0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
    0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
    0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
    0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
    0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
    0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
    0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
    0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
    0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
    0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
    0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
    0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
    0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,
    0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,
    0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,
    0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,
    0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,
    0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,
    0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,
    0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,
    0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,
    0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,
    0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,
    0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25,  0.25
  };
  float *fa_ptr= float_array;
  /* Number of columns per row */
  const int COLUMN_COUNT= sizeof(float_array)/sizeof(*float_array);
  /* Number of rows per bulk insert to start with */
  const int MIN_ROWS_PER_INSERT= 262;
  /* Max number of rows per bulk insert to end with */
  const int MAX_ROWS_PER_INSERT= 300;
  const int MAX_PARAM_COUNT= COLUMN_COUNT*MAX_ROWS_PER_INSERT;
  const char *query_template= "insert into t1 values %s";
  const int CHARS_PER_PARAM= 5; /* space needed to place ", ?" in the query */
  const int uint16_max= 65535;
  int nrows, i;

  myheader("test_bug5194");

  stmt_text= "drop table if exists t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));

  stmt_text= "create table if not exists t1"
   "(c1 float, c2 float, c3 float, c4 float, c5 float, c6 float, "
   "c7 float, c8 float, c9 float, c10 float, c11 float, c12 float, "
   "c13 float, c14 float, c15 float, c16 float, c17 float, c18 float, "
   "c19 float, c20 float, c21 float, c22 float, c23 float, c24 float, "
   "c25 float, c26 float, c27 float, c28 float, c29 float, c30 float, "
   "c31 float, c32 float, c33 float, c34 float, c35 float, c36 float, "
   "c37 float, c38 float, c39 float, c40 float, c41 float, c42 float, "
   "c43 float, c44 float, c45 float, c46 float, c47 float, c48 float, "
   "c49 float, c50 float, c51 float, c52 float, c53 float, c54 float, "
   "c55 float, c56 float, c57 float, c58 float, c59 float, c60 float, "
   "c61 float, c62 float, c63 float, c64 float, c65 float, c66 float, "
   "c67 float, c68 float, c69 float, c70 float, c71 float, c72 float, "
   "c73 float, c74 float, c75 float, c76 float, c77 float, c78 float, "
   "c79 float, c80 float, c81 float, c82 float, c83 float, c84 float, "
   "c85 float, c86 float, c87 float, c88 float, c89 float, c90 float, "
   "c91 float, c92 float, c93 float, c94 float, c95 float, c96 float, "
   "c97 float, c98 float, c99 float, c100 float, c101 float, c102 float, "
   "c103 float, c104 float, c105 float, c106 float, c107 float, c108 float, "
   "c109 float, c110 float, c111 float, c112 float, c113 float, c114 float, "
   "c115 float, c116 float, c117 float, c118 float, c119 float, c120 float, "
   "c121 float, c122 float, c123 float, c124 float, c125 float, c126 float, "
   "c127 float, c128 float, c129 float, c130 float, c131 float, c132 float, "
   "c133 float, c134 float, c135 float, c136 float, c137 float, c138 float, "
   "c139 float, c140 float, c141 float, c142 float, c143 float, c144 float, "
   "c145 float, c146 float, c147 float, c148 float, c149 float, c150 float, "
   "c151 float, c152 float, c153 float, c154 float, c155 float, c156 float, "
   "c157 float, c158 float, c159 float, c160 float, c161 float, c162 float, "
   "c163 float, c164 float, c165 float, c166 float, c167 float, c168 float, "
   "c169 float, c170 float, c171 float, c172 float, c173 float, c174 float, "
   "c175 float, c176 float, c177 float, c178 float, c179 float, c180 float, "
   "c181 float, c182 float, c183 float, c184 float, c185 float, c186 float, "
   "c187 float, c188 float, c189 float, c190 float, c191 float, c192 float, "
   "c193 float, c194 float, c195 float, c196 float, c197 float, c198 float, "
   "c199 float, c200 float, c201 float, c202 float, c203 float, c204 float, "
   "c205 float, c206 float, c207 float, c208 float, c209 float, c210 float, "
   "c211 float, c212 float, c213 float, c214 float, c215 float, c216 float, "
   "c217 float, c218 float, c219 float, c220 float, c221 float, c222 float, "
   "c223 float, c224 float, c225 float, c226 float, c227 float, c228 float, "
   "c229 float, c230 float, c231 float, c232 float, c233 float, c234 float, "
   "c235 float, c236 float, c237 float, c238 float, c239 float, c240 float, "
   "c241 float, c242 float, c243 float, c244 float, c245 float, c246 float, "
   "c247 float, c248 float, c249 float, c250 float)";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  my_bind= (MYSQL_BIND*) malloc(MAX_PARAM_COUNT * sizeof(MYSQL_BIND));
  query= (char*) malloc(strlen(query_template) +
                        MAX_PARAM_COUNT * CHARS_PER_PARAM + 1);
  param_str= (char*) malloc(COLUMN_COUNT * CHARS_PER_PARAM);

  if (my_bind == 0 || query == 0 || param_str == 0)
  {
    fprintf(stderr, "Can't allocate enough memory for query structs\n");
    if (my_bind)
      free(my_bind);
    if (query)
      free(query);
    if (param_str)
      free(param_str);
    return;
  }

  stmt= mysql_stmt_init(mysql);

  /* setup a template for one row of parameters */
  sprintf(param_str, "(");
  for (i= 1; i < COLUMN_COUNT; ++i)
    strcat(param_str, "?, ");
  strcat(param_str, "?)");
  param_str_length= strlen(param_str);

  /* setup bind array */
  bzero((char*) my_bind, MAX_PARAM_COUNT * sizeof(MYSQL_BIND));
  for (i= 0; i < MAX_PARAM_COUNT; ++i)
  {
    my_bind[i].buffer_type= MYSQL_TYPE_FLOAT;
    my_bind[i].buffer= fa_ptr;
    if (++fa_ptr == float_array + COLUMN_COUNT)
      fa_ptr= float_array;
  }

  /*
    Test each number of rows per bulk insert, so that we can see where
    MySQL fails.
  */
  for (nrows= MIN_ROWS_PER_INSERT; nrows <= MAX_ROWS_PER_INSERT; ++nrows)
  {
    char *query_ptr;
    /* Create statement text for current number of rows */
    sprintf(query, query_template, param_str);
    query_ptr= query + strlen(query);
    for (i= 1; i < nrows; ++i)
    {
      memcpy(query_ptr, ", ", 2);
      query_ptr+= 2;
      memcpy(query_ptr, param_str, param_str_length);
      query_ptr+= param_str_length;
    }
    *query_ptr= '\0';

    rc= mysql_stmt_prepare(stmt, query, query_ptr - query);
    if (rc && nrows * COLUMN_COUNT > uint16_max)
    {
      if (!opt_silent)
        printf("Failed to prepare a statement with %d placeholders "
               "(as expected).\n", nrows * COLUMN_COUNT);
      break;
    }
    else
      check_execute(stmt, rc);

    if (!opt_silent)
      printf("Insert: query length= %d, row count= %d, param count= %lu\n",
             (int) strlen(query), nrows, mysql_stmt_param_count(stmt));

    /* bind the parameter array and execute the query */
    rc= mysql_stmt_bind_param(stmt, my_bind);
    check_execute(stmt, rc);

    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
    mysql_stmt_reset(stmt);
  }

  mysql_stmt_close(stmt);
  free(my_bind);
  free(query);
  free(param_str);
  stmt_text= "drop table t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
}