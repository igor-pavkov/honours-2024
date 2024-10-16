void HtmlOutputDev::dumpMetaVars(FILE *file)
{
  GooString *var;

  for(std::size_t i = 0; i < glMetaVars->size(); i++)
  {
     HtmlMetaVar *t = (*glMetaVars)[i];
     var = t->toString(); 
     fprintf(file, "%s\n", var->c_str());
     delete var;
  }
}