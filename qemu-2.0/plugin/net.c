/*
 * Networking of guest VM (Linux)
 *
 * Copyright Dongli Zhang, 2014
 *
 * Authors: 
 *  Dongli Zhang   <dongli.zhang0129@gmail.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "symbol-x86_64.h"
#include "linux-x86_64.h"
#include "net.h"
#include "list.h"
#include "memory.h"
#include "tasks.h"

// obtain network device info
void print_net_device(Monitor *mon, void *mem)
{
	struct list_head *dev_base_head_hva = (mem+virt_to_phys(DEV_BASE_HEAD_GVA));
	unsigned long net_list_gva = (unsigned long)(dev_base_head_hva->next);
	unsigned long net_list_gpa;

	struct list_head *net_list_hva;
	void *net_base_hva;

	while(net_list_gva != DEV_BASE_HEAD_GVA)
	{
		net_list_gpa = virt_to_phys(net_list_gva);
		net_base_hva = mem+net_list_gpa-ND_DEVICE_LIST_OFFSET;
		
		char *name = (char *)(net_base_hva+ND_NAME_OFFSET);
		monitor_printf(mon, "Device: %s\n", name);

		unsigned long dev_addr_gva = *(unsigned long *)(net_base_hva+ND_DEV_ADDR_OFFSET);
		unsigned char *dev_addr_hva = (unsigned char *)(mem+virt_to_phys(dev_addr_gva));
		
		// mac address
		monitor_printf(mon, "   HWaddr     : %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X\n",
				dev_addr_hva[0], dev_addr_hva[1],
				dev_addr_hva[2], dev_addr_hva[3],
				dev_addr_hva[4], dev_addr_hva[5]);

		unsigned long ind_gva = *(unsigned long *)(net_base_hva+ND_IP_PTR_OFFSET);
		if(ind_gva != 0x0)
		{
			void *ind_hva = (void *)(mem+virt_to_phys(ind_gva));
			unsigned long ina_gva = *(unsigned long *)(ind_hva+IND_IFA_LIST_OFFSET);

			if(ina_gva != 0x0)
			{
				void *ina_hva = (void *)(mem+virt_to_phys(ina_gva));
				// ip address
				unsigned int ip_net = *(unsigned int *)(ina_hva+INA_IFA_ADDRESS_OFFSET);
				// broadcast address
				unsigned int broadcast_net = *(unsigned int *)(ina_hva+INA_IFA_BROADCAST_OFFSET);
				// address mask
				unsigned int mask_net = *(unsigned int *)(ina_hva+INA_IFA_MASK_OFFSET);
				
				char ip[IP_LEN];
				char broadcast[IP_LEN];
				char mask[IP_LEN];

				struct in_addr ia;
			
				// ip address
				ia.s_addr = ip_net;
				sprintf(ip, "%s", inet_ntoa(ia));
			
				// broadcast address
				ia.s_addr = broadcast_net;
				sprintf(broadcast, "%s", inet_ntoa(ia));
			
				// address mask
				ia.s_addr = mask_net;
				sprintf(mask, "%s", inet_ntoa(ia));

				monitor_printf(mon, "   IP Address : %s\n", ip);
				monitor_printf(mon, "   Broadcast  : %s\n", broadcast);
				monitor_printf(mon, "   IP Mask    : %s\n", mask);
			}
		}

		// next
		net_list_hva = (struct list_head *)(mem+net_list_gpa);
		net_list_gva = (unsigned long)(net_list_hva->next);
	}
}
