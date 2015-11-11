#include <stdio.h>
#include <sys/io.h>
#include <errno.h>
#include <stdlib.h>

#include "pci_constants.h"
#include "pci.h"

char *GetVendorName(int vendorID)
{
	int i;
	for (i = 0; i < PCI_VENTABLE_LEN; i++)
		if (PciVenTable[i].VendorId == vendorID)
			return PciVenTable[i].VendorName;
	return NOT_FOUND;	
}

char *GetDeviceName(int vendorID, int deviceID)
{
	int i;
	for (i = 0; i < PCI_DEVTABLE_LEN; i++)
		if (PciDevTable[i].VendorId == vendorID && PciDevTable[i].DeviceId == deviceID)
			return PciDevTable[i].DeviceName;	
	
	return NOT_FOUND;
}

ulong GetDataFromRegister(ushort bus, ushort device, ushort function, ushort registerNumber)
{
	ulong command = CONTROL_BIT | (bus << BUS_SHIFT) | (device << DEVICE_SHIFT) | (function << FUNCTION_SHIFT) | (registerNumber << REGISTER_SHIFT);
	outl(command, CONTROL_PORT);
	return inl(INFO_PORT);	
}

ulong GetIDs(ushort bus, ushort device, ushort function)
{
	return GetDataFromRegister(bus, device, function, 0);
}

byte GetHeaderType(ushort bus, ushort device, ushort function)
{
	ulong answer = GetDataFromRegister(bus, device, function, HEADER_TYPE_REGISTER);
	return (answer >> 16) & 255;	
}


void PrintDeviceMainInfo(ushort deviceID, ushort vendorID)
{
	printf("Vendor ID: %x ", vendorID);
	puts(GetVendorName(vendorID));
		
	printf("Device ID: %x ", deviceID);
	puts(GetDeviceName(vendorID, deviceID));	
}

ulong GetClassCode(ushort bus, ushort device, ushort function)
{
	return GetDataFromRegister(bus, device, function, 0x08) >> 8;
}

char *GetBaseClassDescription(byte baseClass)
{
	int i;
	for (i = 0; i < PCI_CLASSCODETABLE_LEN; i++)
		if (baseClass == PciClassCodeTable[i].BaseClass)
			return PciClassCodeTable[i].BaseDesc;
	
	return NOT_FOUND;	
}

char *GetSubclassDescription(byte baseClass, byte subclass)
{
	int i;
	for (i = 0; i < PCI_CLASSCODETABLE_LEN; i++)
		if (subclass == PciClassCodeTable[i].SubClass && baseClass == PciClassCodeTable[i].BaseClass)
			return PciClassCodeTable[i].SubDesc;
	
	return NOT_FOUND;	
}

char *GetProgrammingInterfaceDescription(byte baseClass, byte subclass, byte programmingInterface)
{
	int i;
	for (i = 0; i < PCI_CLASSCODETABLE_LEN; i++)
		if (subclass == PciClassCodeTable[i].SubClass && 
			baseClass == PciClassCodeTable[i].BaseClass && 
			programmingInterface == PciClassCodeTable[i].ProgIf)
			
			return PciClassCodeTable[i].ProgDesc;
	
	return NOT_FOUND;	
}

void PrintClassCodeInfo(ulong registerData)
{	
	ulong classCode = registerData >> 8;
	byte baseClass = (classCode >> 16) & 255;
	byte subclass = (classCode >> 8) & 255;
	byte programmingInterface = classCode & 255;
	
	printf("Class Code: %#x\n", classCode);
	
	printf("Base class: %#x ", baseClass);
	puts(GetBaseClassDescription(baseClass));
	
	printf("Subclass: %#x ", subclass);
	puts(GetSubclassDescription(baseClass, subclass));
	
	printf("Specific register level programming interface : %#x ", programmingInterface);
	puts(GetProgrammingInterfaceDescription(baseClass, subclass, programmingInterface));	
}

void PrintBARInfo(ulong registerInfo)
{
	if (!(registerInfo & 1))
	{
		puts("Memory");
		switch ((registerInfo >> 1) & 3)
		{
			case 0:
				puts("Any place in 32-bit address space.");
				break;
			case 1:
				puts("Only < 1MB");
				break;
			case 2:
				puts("Any place in 64-bit address space");
				break;
			default:
				break;
		}
		printf("Address: %#x\n", registerInfo >> 4);
				
	}
	else
	{
		puts("I\\O");
		printf("Address: %#x\n", registerInfo >> 2);
	}
}

void ProcessBARs(ushort bus, ushort device, ushort function)
{
	int i;
	puts("\nBAR info");
	for ( i = 4; i < 10; i++)
	{
		printf("Register %d:\n", i - 3);
		ulong registerInfo = GetDataFromRegister(bus, device, function, i);
		if (registerInfo)
			PrintBARInfo(registerInfo);
		else
			puts("Not used");
	}
	
}

void PrintExpansionROMInfo(ulong registerInfo)
{
	printf("\nExpansion ROM base address: ");
	if (registerInfo & 1)
	{
		puts("can be used");
		printf("Address: %#x\n\n", registerInfo >> 11);	
	}
	else
		puts("can't be used\n");
	
}

void PrintCacheLineSizeInfo(ulong registerInfo)
{
	byte cacheLineSize = registerInfo & 255;
	printf("Cache Line Size: ");
	if (cacheLineSize % 2)
		puts("0\n");
	else
		printf("%d\n\n", cacheLineSize);	
}

void PrintIOInfo(ulong registerData)
{
	byte IOBase = registerData & 255;
	byte IOLimit = (registerData >> 8) & 255;
	printf("I/O Base: %#x\n", IOBase);	
	printf("I/O Limit: %#x\n\n", IOLimit);
}

void PrintMemoryInfo(ulong registerData)
{
	ushort memoryBase = registerData && 65535;
	ushort memoryLimit = registerData >> 16;
	
	printf("Memory Base: %#x\n", memoryBase);
	printf("Memory Limit: %#x\n\n", memoryLimit);
}

void PrintInterruptPinInfo(ulong registerData)
{
	printf("Interrupt Pin: ");
	byte interruptPin = (registerData >> 8) & 255;
	switch (interruptPin)
	{
		case 0:
			puts("not used");
			break;
		case 1:
			puts("INTA#");
			break;
		case 2:
			puts("INTB#");
			break;
		case 3:
			puts("INTC#");
			break;
		case 4:
			puts("INTD#");
			break;
		default:
			break;
	}
	puts("");
}

void PrintInterruptLineInfo(ulong registerData)
{
	printf("Interrupt Line: ");
	byte interruptLine = registerData & 255;
	if (interruptLine == 255)
		puts("unknown or not used\n");
	else
		printf("IRQ%u\n\n", interruptLine);	
}

void PrintBusNumbersInfo(ulong registerData)
{
	printf("\nPrimary Bus Number: %#x\n", registerData & 255);
	printf("Secondary Bus Number: %#x\n", (registerData >> 8) & 255);
	printf("Subordinate Bus Number: %#x\n\n", (registerData >> 16) & 255); 
}

void ProcessDevice(ushort bus, ushort device, ushort function)
{
	
	ulong IDs = GetIDs(bus, device, function);
	
	if (IDs != 0xffffffff)
	{
		ushort deviceID = IDs >> 16;
		ushort vendorID = IDs & 65535;
		printf("%x.%x.%x\n", bus, device, function);
		PrintDeviceMainInfo(deviceID, vendorID);	
		
		byte isBridge = (GetHeaderType(bus, device, function) >> 7) & 1;
		
		if (isBridge)
			puts("\nBridge\n");
		else
			puts("\nNot a bridge\n");
		
		PrintClassCodeInfo(GetDataFromRegister(bus, device, function, CLASS_CODE_REGISTER));
				
		if (isBridge)
		{
			PrintBusNumbersInfo(GetDataFromRegister(bus, device, function, BUS_NUMBERS_REGISTER));
			PrintIOInfo(GetDataFromRegister(bus, device, function, IO_REGISTER));
			PrintMemoryInfo(GetDataFromRegister(bus, device, function, MEMORY_REGISTER));
		}
		else
		{
			ProcessBARs(bus, device, function);
			PrintExpansionROMInfo(GetDataFromRegister(bus, device, function, EXPANSION_ROM_REGISTER));
			PrintCacheLineSizeInfo(GetDataFromRegister(bus, device, function, CACHE_LINE_SIZE_REGISTER));
		}	
		
		PrintInterruptPinInfo(GetDataFromRegister(bus, device, function, INTERRPUT_PIN_REGISTER));
		PrintInterruptLineInfo(GetDataFromRegister(bus, device, function, INTERRPUT_LINE_REGISTER));
		
		puts("-------------------");	
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
		for (device = 0; device < 32; device++)
			for (function = 0; function < 8; function++)
				ProcessDevice(bus, device, function);
	
	puts("Press any key...");
	getchar();
	
	return 0;
}
