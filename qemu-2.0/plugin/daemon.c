/*
 * The listener to generate xml
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
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "daemon.h"
#include "configuration.h"
#include "mem_usage.h"
#include "cpu_usage.h"

// pthread thread function of the listener
void * thread_listen(void *arg);
void listener_service(Monitor *mon, void *mem);
char* gen_xml(void *mem);

#define SRV_PORT 50001
#define TMP_XML_FILE "/tmp/qemuxml.xml"

// params to thread func
struct listener_info
{
	Monitor *mon;
	void *mem;
};

pthread_t listen_tid;
static int is_listener_started = 0;
int listenfd;

char* gen_xml(void *mem)
{
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
 
	// create new XML document
	doc = xmlNewDoc(BAD_CAST "1.0");
	// create root of XML
	root = xmlNewNode(NULL, BAD_CAST "info");
	xmlDocSetRootElement(doc, root);

	// generate content of xml
	char tmp[128];
	// hostname
	struct hostname_stat hstat;
	get_hostname(mem, &hstat);
	xmlNodePtr host_node = xmlNewNode(NULL, BAD_CAST "host");
	xmlAddChild(root, host_node);
	xmlNewChild(host_node, NULL, BAD_CAST "sysname",    BAD_CAST hstat.sysname);
	xmlNewChild(host_node, NULL, BAD_CAST "nodename",   BAD_CAST hstat.nodename);
	xmlNewChild(host_node, NULL, BAD_CAST "release",    BAD_CAST hstat.release);
	xmlNewChild(host_node, NULL, BAD_CAST "version",    BAD_CAST hstat.version);
	xmlNewChild(host_node, NULL, BAD_CAST "machine",    BAD_CAST hstat.machine);
	xmlNewChild(host_node, NULL, BAD_CAST "domainname", BAD_CAST hstat.domainname);

	// cpu usage
	int num_cpu = get_cpu_num(mem);
	struct cpu_usage_stat cstat[num_cpu+1];
	comp_cpu_usage(mem, cstat);
	xmlNodePtr cpu_node = xmlNewNode(NULL, BAD_CAST "cpu-usage");
	xmlAddChild(root, cpu_node);

	xmlNodePtr avg_node = xmlNewNode(NULL, BAD_CAST "average");
	xmlAddChild(cpu_node, avg_node);

	memset(tmp, 0, 128);
	sprintf(tmp, "%.1f%%", cstat[num_cpu].usage[CPUTIME_USER]);
	xmlNewChild(avg_node, NULL, BAD_CAST "US", BAD_CAST tmp);
		
	memset(tmp, 0, 128);
	sprintf(tmp, "%.1f%%", cstat[num_cpu].usage[CPUTIME_SYSTEM]);
	xmlNewChild(avg_node, NULL, BAD_CAST "SY", BAD_CAST tmp);
		
	memset(tmp, 0, 128);
	sprintf(tmp, "%.1f%%", cstat[num_cpu].usage[CPUTIME_IDLE]);
	xmlNewChild(avg_node, NULL, BAD_CAST "ID", BAD_CAST tmp);
		
	memset(tmp, 0, 128);
	sprintf(tmp, "%.1f%%", cstat[num_cpu].usage[CPUTIME_NICE]);
	xmlNewChild(avg_node, NULL, BAD_CAST "NI", BAD_CAST tmp);
		
	memset(tmp, 0, 128);
	sprintf(tmp, "%.1f%%", cstat[num_cpu].usage[CPUTIME_IOWAIT]);
	xmlNewChild(avg_node, NULL, BAD_CAST "WA", BAD_CAST tmp);
		
	memset(tmp, 0, 128);
	sprintf(tmp, "%.1f%%", cstat[num_cpu].usage[CPUTIME_IRQ]);
	xmlNewChild(avg_node, NULL, BAD_CAST "HI", BAD_CAST tmp);
		
	memset(tmp, 0, 128);
	sprintf(tmp, "%.1f%%", cstat[num_cpu].usage[CPUTIME_SOFTIRQ]);
	xmlNewChild(avg_node, NULL, BAD_CAST "SI", BAD_CAST tmp);
		
	memset(tmp, 0, 128);
	sprintf(tmp, "%.1f%%", cstat[num_cpu].usage[CPUTIME_STEAL]);
	xmlNewChild(avg_node, NULL, BAD_CAST "ST", BAD_CAST tmp);

	int c;
	for(c=0; c<num_cpu; c++)
	{
		xmlNodePtr cnode = xmlNewNode(NULL, BAD_CAST "cpu");
		xmlAddChild(cpu_node, cnode);

		memset(tmp, 0, 128);
		sprintf(tmp, "%d", c);
		xmlNewProp(cnode, BAD_CAST "index", BAD_CAST tmp);

		memset(tmp, 0, 128);
		sprintf(tmp, "%.1f%%", cstat[c].usage[CPUTIME_USER]);
		xmlNewChild(cnode, NULL, BAD_CAST "US", BAD_CAST tmp);
		
		memset(tmp, 0, 128);
		sprintf(tmp, "%.1f%%", cstat[c].usage[CPUTIME_SYSTEM]);
		xmlNewChild(cnode, NULL, BAD_CAST "SY", BAD_CAST tmp);
		
		memset(tmp, 0, 128);
		sprintf(tmp, "%.1f%%", cstat[c].usage[CPUTIME_IDLE]);
		xmlNewChild(cnode, NULL, BAD_CAST "ID", BAD_CAST tmp);
		
		memset(tmp, 0, 128);
		sprintf(tmp, "%.1f%%", cstat[c].usage[CPUTIME_NICE]);
		xmlNewChild(cnode, NULL, BAD_CAST "NI", BAD_CAST tmp);
		
		memset(tmp, 0, 128);
		sprintf(tmp, "%.1f%%", cstat[c].usage[CPUTIME_IOWAIT]);
		xmlNewChild(cnode, NULL, BAD_CAST "WA", BAD_CAST tmp);
		
		memset(tmp, 0, 128);
		sprintf(tmp, "%.1f%%", cstat[c].usage[CPUTIME_IRQ]);
		xmlNewChild(cnode, NULL, BAD_CAST "HI", BAD_CAST tmp);
		
		memset(tmp, 0, 128);
		sprintf(tmp, "%.1f%%", cstat[c].usage[CPUTIME_SOFTIRQ]);
		xmlNewChild(cnode, NULL, BAD_CAST "SI", BAD_CAST tmp);
		
		memset(tmp, 0, 128);
		sprintf(tmp, "%.1f%%", cstat[c].usage[CPUTIME_STEAL]);
		xmlNewChild(cnode, NULL, BAD_CAST "ST", BAD_CAST tmp);
	}

	// memory usage
	struct mem_usage_stat mstat;
	get_mem_usage(mem, &mstat);
	xmlNodePtr mem_node = xmlNewNode(NULL, BAD_CAST "mem-usage");
	xmlAddChild(root, mem_node);
	
	memset(tmp, 0, 128);
	sprintf(tmp, "%lu kB", mstat.MemTotal);
	xmlNewChild(mem_node, NULL, BAD_CAST "MemTotal", BAD_CAST tmp);
	
	memset(tmp, 0, 128);
	sprintf(tmp, "%lu kB", mstat.MemFree);
	xmlNewChild(mem_node, NULL, BAD_CAST "MemTotal", BAD_CAST tmp);
	
	memset(tmp, 0, 128);
	sprintf(tmp, "%lu kB", mstat.Buffers);
	xmlNewChild(mem_node, NULL, BAD_CAST "MemTotal", BAD_CAST tmp);

	memset(tmp, 0, 128);
	sprintf(tmp, "%lu kB", mstat.Cached);
	xmlNewChild(mem_node, NULL, BAD_CAST "MemTotal", BAD_CAST tmp);

	memset(tmp, 0, 128);
	sprintf(tmp, "%.1f%%", mstat.mem_usage);
	xmlNewChild(mem_node, NULL, BAD_CAST "MemTotal", BAD_CAST tmp);

	// save the xml to file of size 'size'
	int size = xmlSaveFormatFileEnc(TMP_XML_FILE, doc, "UTF-8", 1);

	// free the document
	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();
 
	// read from the file
	int fd = open(TMP_XML_FILE, O_RDONLY);
	if(fd < 0)
	{
		perror("Cannot open file\n");
		return NULL;
	}
  
	char *buf = (char *)malloc(sizeof(char)*(size+1));
	memset(buf, 0, sizeof(char)*(size+1));
	int i;

	if((i=read(fd, buf, sizeof(char)*(size+1))) < 0)
		perror("error\n");
	
	close(fd);
	return buf;
}

void listener_service(Monitor *mon, void *mem)
{
	int sockfd;
	struct ifreq ifr;
	const char *ifname = "eth0";

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ-1);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(ioctl(sockfd, SIOCGIFADDR, &ifr) == -1)
	{
		monitor_printf(mon, "failed to obtain the ip address\n");
	}
	else
	{
		struct sockaddr_in *myaddr;
		myaddr = (struct sockaddr_in*)&(ifr.ifr_addr);
		monitor_printf(mon, "Please connect to IP: %s:%d\n", inet_ntoa(myaddr->sin_addr), SRV_PORT);
	}
	
	monitor_printf(mon, "(qemu) ");
	monitor_flush(mon);

	int connfd;
	struct sockaddr_in srvaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
	{
		perror("Cannot create socket\n");
		return;
	}

	bzero(&srvaddr, sizeof(srvaddr));
	srvaddr.sin_family = AF_INET;
	srvaddr.sin_addr.s_addr = INADDR_ANY;
	srvaddr.sin_port = htons(SRV_PORT);

	int opt = 1;
	int len = sizeof(opt);
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, len);

	if(bind(listenfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) < 0)
	{
		perror("Cannot bind\n");
		return;
	}

	listen(listenfd, 128);

	while(1)
	{
		connfd = accept(listenfd, NULL, NULL);

		char *buf = gen_xml(mem);

		if(write(connfd, buf, strlen(buf)) < 0)
		{
			perror("write error\n");
		}

		close(connfd);
		free(buf);
	}

	close(listenfd);
}

// pthread thread function to check integrity of system call table
void * thread_listen(void *arg)
{
	struct listener_info *info = (struct listener_info *)arg;
	void *mem = info->mem;
	Monitor *mon = info->mon;

	listener_service(mon, mem);
	return NULL;
}

// start the listener by creating a new thread
void listener_start(Monitor *mon, void *mem)
{
	// return if the listener already started
	if(is_listener_started == 1)
	{
		monitor_printf(mon, "error: listener already started.\n");
		return;
	}

	struct listener_info *info = (struct listener_info *)malloc(sizeof(struct listener_info));
	info->mem = mem;
	info->mon = mon;

	// create the pthread thread 
	if(pthread_create(&listen_tid, NULL, thread_listen, (void *)info) != 0)
	{
		printf("Failed to create thread\n");
		return;
	}

	is_listener_started = 1;
	monitor_printf(mon, "listener started.\n\n");
}

// stop the listener
void listener_stop(Monitor *mon)
{
	// return if the listener is not started yet
	if(is_listener_started == 0)
	{
		monitor_printf(mon, "listener NOT started.\n");
		return;
	}

	if(pthread_cancel(listen_tid) != 0)
	{
		printf("Failed to cancel thread\n");
		return;
	}
	 
	if(pthread_join(listen_tid, NULL) != 0)
	{
		printf("Failed to join thread\n");
		return;
	}

	close(listenfd);
	is_listener_started = 0;
	monitor_printf(mon, "listener stopped.\n");
}
