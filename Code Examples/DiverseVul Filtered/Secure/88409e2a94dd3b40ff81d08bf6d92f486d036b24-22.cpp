std::vector<std::string> EbmlMaster::FindAllMissingElements()
{
  assert(Context.GetSize() != 0);

  std::vector<std::string> missingElements;

  for (size_t ChildElementNo = 0; ChildElementNo < ElementList.size(); ChildElementNo++) {
       EbmlElement *childElement = ElementList[ChildElementNo];
    if (!childElement->ValueIsSet()) {
      std::string missingValue;
      missingValue = "The Child Element \"";
      missingValue.append(EBML_NAME(childElement));
      missingValue.append("\" of EbmlMaster \"");
      missingValue.append(EBML_NAME(this));
      missingValue.append("\", does not have a value set.");
      missingElements.push_back(missingValue);
    }

    if (childElement->IsMaster()) {
      EbmlMaster *childMaster = (EbmlMaster *)childElement;

      std::vector<std::string> childMissingElements = childMaster->FindAllMissingElements();
      for (size_t s = 0; s < childMissingElements.size(); s++)
        missingElements.push_back(childMissingElements[s]);
    }
  }
  unsigned int EltIdx;
  for (EltIdx = 0; EltIdx < EBML_CTX_SIZE(Context); EltIdx++) {
    if (EBML_CTX_IDX(Context,EltIdx).IsMandatory()) {
      if (FindElt(EBML_CTX_IDX_INFO(Context,EltIdx)) == NULL) {
        std::string missingElement;
        missingElement = "Missing element \"";
                missingElement.append(EBML_INFO_NAME(EBML_CTX_IDX_INFO(Context,EltIdx)));
        missingElement.append("\" in EbmlMaster \"");
                missingElement.append(EBML_INFO_NAME(*EBML_CTX_MASTER(Context)));
        missingElement.append("\"");
        missingElements.push_back(missingElement);
      }
    }
  }

  return missingElements;
}