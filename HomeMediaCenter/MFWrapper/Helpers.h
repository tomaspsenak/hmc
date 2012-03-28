#pragma once

#if !defined(HELPERS_MFWRAPPER_INCLUDED)
#define HELPERS_MFWRAPPER_INCLUDED

System::Guid FromGUID(GUID inguid) 
{
   return System::Guid( inguid.Data1, inguid.Data2, inguid.Data3, 
                inguid.Data4[0], inguid.Data4[1], 
                inguid.Data4[2], inguid.Data4[3], 
                inguid.Data4[4], inguid.Data4[5], 
                inguid.Data4[6], inguid.Data4[7] );
}

GUID ToGUID(System::Guid inguid) 
{
   array<System::Byte>^ guidData = inguid.ToByteArray();
   pin_ptr<System::Byte> data = &(guidData[0]);

   return *(GUID*)data;
}

#endif //HELPERS_MFWRAPPER_INCLUDED

