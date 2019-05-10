#include <linux/fs.h> 
#include <linux/nsproxy.h> 
#include <linux/dcache.h> 
#include <linux/types.h> 
#include <asm/byteorder.h> 
#include <linux/socket.h> 
#include <linux/ip.h> 
#include <linux/tcp.h> 
#include <linux/skbuff.h> 
#include <linux/module.h> 
#include <linux/moduleparam.h> 
#include <linux/kernel.h> 
#include <linux/init.h> 


MODULE_LICENSE("GPL"); 

int init_module(void) { 
printk(KERN_INFO "Starting"); 
struct iphdr *ip_header; // ip header struct 
struct tcphdr *tcp_header; // tcp header struct 
//struct udphdr *udp_header; // udp header struct 
struct sk_buff *sock_buff; 

unsigned int sport, dport; 


// sock_buff = skb; 

if (!sock_buff) 
return NF_ACCEPT; 

ip_header = (struct iphdr *)skb_network_header(sock_buff); 
if (!ip_header) 
return NF_ACCEPT; 


//if TCP PACKET 
if(ip_header->protocol==IPPROTO_TCP) 
{ 
//tcp_header = (struct tcphdr *)skb_transport_header(sock_buff); //doing the cast this way gave me the same problem 

tcp_header= (struct tcphdr *)((__u32 *)ip_header+ ip_header->ihl); //this fixed the problem 

sport = htons((unsigned short int) tcp_header->source); //sport now has the source port 
dport = htons((unsigned short int) tcp_header->dest); //dport now has the dest port 
printk(KERN_INFO "Source port %d", sport); 
printk(KERN_INFO "Destination port %d", dport); 
} 
return 0; 
} 

void cleanup_module(void) { 
printk(KERN_INFO "Cleanup"); 
}

