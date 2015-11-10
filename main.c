#include <stdio.h>
#include <sys/io.h>
#include <errno.h>
#include "pci.h"
#include <stdlib.h>

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


int main(int argc, char **argv)
{
	if (iopl(3) )
	{
		printf("I/O Privilege level change error\n Try run under ROOT rights\n");
		return 1;
	}
	
	unsigned short busID, deviceID, functionID;
	unsigned long command = 0, answer = 0;
	
	for (busID = 0; busID < 256; busID++)
	{
		for (deviceID = 0; deviceID < 32; deviceID++)
		{
			for (functionID = 0; functionID < 8; functionID++)
			{
				command = (1 << 31) | (busID << 16) | (deviceID << 11) | (functionID << 8);
				outl(command, 0x0CF8);
				answer = inl(0x0CFC);

				unsigned short deviceID = answer >> 16;
				unsigned short vendorID = answer & 65535;
				if (answer != 0xffffffff)
				{
					printf("%#x.%#x.%#x\n", busID, deviceID, functionID);
					
					char *vendorName = GetVendorName(vendorID);
					char *deviceName = GetDeviceName(vendorID, deviceID);
					
					if (vendorName)
						printf("%s\n", GetVendorName(vendorID));
					else
						puts("Vendor name not found.");
						
					if (deviceName)
						printf("%s\n\n", GetDeviceName(vendorID, deviceID));
					else
						puts("Device name not found.");
					
				}
			}
			
		}
	}
	
	return 0;
}



	
