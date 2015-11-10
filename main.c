#include <sys/io.h>
#include <errno.h>
#include "pci.h"

int main()
{
	//printf("Peripherial devs lab3\n\n");
	int i,busid,devid;
	
	if(iopl(3)) //задание уровня приоритета
	{
		printf("I/O Privilege level change error: %s\nTry running under ROOT user\n",(char *)strerror(errno));
		return 1;
	} //granted privileges 3 for port access
	
	for (busid=0; busid < 256; busid++)
	{
		for (devid=0; devid < 32; devid++)
		{
			int recvp, ven_id, dev_id, clid;
			int sendp=(1 << 31) | (busid << 16) | (devid << 11);
			outl(sendp, 0x0CF8);
			recvp=inl(0x0CFC);
		}
	}

	
	return 0;
}
	
