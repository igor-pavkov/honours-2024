static inline void CleanupPDFInfo(PDFInfo *pdf_info)
{
  if (pdf_info->profile != (StringInfo *) NULL)
    pdf_info->profile=DestroyStringInfo(pdf_info->profile);
}