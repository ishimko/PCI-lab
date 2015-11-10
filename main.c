#include <stdio.h>
#include <sys/io.h>
#include <errno.h>
#include "pci.h"

char *GetVendorName(int vendorID)
{
	int i;
	for (i = 0; i < PCI_VENTABLE_LEN; i++)
		if (PciVenTable[i].VendorId == vendorID)
			return PciVenTable[i].VendorName;
	return NULL;	
}

char *GetDeviceName(int vendorID, int deviceID)
{
	int i;
	for (i = 0; i < PCI_DEVTABLE_LEN; i++)
		if (PciDevTable[i].VendorId == vendorID && PciDevTable[i].DeviceId == deviceID)
			return PciDevTable[i].DeviceName;	
	
	return NULL;
}


int main()
{
	
	if(iopl(3)) //задание уровня приоритета
	{
		printf("I/O Privilege level change error: %s\nTry running under ROOT user\n",(char *)strerror(errno));
		return 1;
	}

	int busID;
	for (busID = 0; busID < 256; busID++)
	{
		int deviceID;
		for (deviceID = 0; deviceID < 32; deviceID++)
		{
			int recvp, ven_id, dev_id, clid;
			int sendp=(1 << 31) | (busid << 16) | (devid << 11);
			outl(sendp, 0x0CF8);
			recvp=inl(0x0CFC);
		}
	}

	
	return 0;
}
	
