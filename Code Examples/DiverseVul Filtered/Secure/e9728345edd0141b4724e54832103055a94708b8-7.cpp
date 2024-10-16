static const char *GetImageTransferCharacteristic(
  const DPXTransferCharacteristic characteristic)
{
  const char
    *transfer;

  /*
    Get the element transfer characteristic.
  */
  switch(characteristic)
  {
    case TransferCharacteristicUserDefined:
    {
      transfer="UserDefined";
      break;
    }
    case TransferCharacteristicPrintingDensity:
    {
      transfer="PrintingDensity";
      break;
    }
    case TransferCharacteristicLinear:
    {
      transfer="Linear";
      break;
    }
    case TransferCharacteristicLogarithmic:
    {
      transfer="Logarithmic";
      break;
    }
    case TransferCharacteristicUnspecifiedVideo:
    {
      transfer="UnspecifiedVideo";
      break;
    }
    case TransferCharacteristicSMTPE274M:
    {
      transfer="SMTPE274M";
      break;
    }
    case TransferCharacteristicITU_R709:
    {
      transfer="ITU-R709";
      break;
    }
    case TransferCharacteristicITU_R601_625L:
    {
      transfer="ITU-R601-625L";
      break;
    }
    case TransferCharacteristicITU_R601_525L:
    {
      transfer="ITU-R601-525L";
      break;
    }
    case TransferCharacteristicNTSCCompositeVideo:
    {
      transfer="NTSCCompositeVideo";
      break;
    }
    case TransferCharacteristicPALCompositeVideo:
    {
      transfer="PALCompositeVideo";
      break;
    }
    case TransferCharacteristicZDepthLinear:
    {
      transfer="ZDepthLinear";
      break;
    }
    case TransferCharacteristicZDepthHomogeneous:
    {
      transfer="ZDepthHomogeneous";
      break;
    }
    default:
      transfer="Reserved";
  }
  return(transfer);
}