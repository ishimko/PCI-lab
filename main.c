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

unsigned

void ProcessDevice(unsigned short bus, unsigned short device, unsigned short function)
{
	unsigned long command = 0, answer = 0;
	command = (1 << 31) | (bus << 16) | (device << 11) | (function << 8);
	outl(command, 0x0CF8);
	answer = inl(0x0CFC);

	unsigned short deviceID = answer >> 16;
	unsigned short vendorID = answer & 65535;
	if (answer != 0xffffffff)
	{
		printf("%u.%u.%u\n", bus, device, function);
		
		char *vendorName = GetVendorName(vendorID);
		char *deviceName = GetDeviceName(vendorID, deviceID);
		
		printf("Vendor ID: %x ", vendorID);
		if (vendorName)
			printf("%s\n", GetVendorName(vendorID));
		else
			puts("Vendor name not found.");
			
		printf("Device ID: %x ", deviceID);
		if (deviceName)
			printf("%s\n\n", GetDeviceName(vendorID, deviceID));
		else
			puts("Device name not found.");
		
	}
	
}

int main(int argc, char **argv)
{
	if (iopl(3) )
	{
		printf("I/O Privilege level change error\n Try run under ROOT rights\n");
		return 1;
	}
	
	unsigned short bus, device, function;
	
	for (bus = 0; bus < 256; bus++)
	{
		for (device = 0; device < 32; device++)
		{
			for (function = 0; function < 8; function++)
			{
				ProcessDevice(bus, device, function);
			}
			
		}
	}
	
	return 0;
}



	
