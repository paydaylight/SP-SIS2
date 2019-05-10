#include <linux/fs.h> 
#include <linux/nsproxy.h> 
#include <linux/dcache.h> 
#include <linux/types.h> 
#include <asm/byteorder.h> 
#include <linux/socket.h> 
#include <linux/ip.h> 
#include <linux/tcp.h> 
#include <linux/udp.h> 
#include <linux/skbuff.h> 
#include <linux/module.h> 
#include <linux/moduleparam.h> 
#include <linux/kernel.h> 
#include <linux/init.h> 


MODULE_LICENSE("GPL"); 

static int __init init(void) 
{ 
        struct iphdr *ip_header; // ip header struct 
        struct tcphdr *tcp_header; // tcp header struct 
        struct udphdr *udp_header; // udp header struct 
        struct sk_buff *sock_buff; 

        unsigned int sport, dport, len; 

        unsigned short proto;

        printk(KERN_INFO "Starting"); 
        
        // sock_buff = skb; 
        
        //sock_buff = alloc_skb(320, GFP_ATOMIC);
        
        sock_buff = alloc_skb(320, GFP_KERNEL);

        printk(KERN_INFO "Uhmm");
        
        if (!sock_buff) {
                printk(KERN_INFO "FFS");
                return NF_ACCEPT; 
        }
        
        printk(KERN_INFO "Wait, that worked??");
        
        skb_put(sock_buff, 320);
        //memcpy(sock_buff -> data, data, 120);
        
        ip_header = (struct iphdr *)skb_network_header(sock_buff); 
        if (!ip_header) {
                printk(KERN_INFO "Empty ip header"); 
                return NF_ACCEPT; 
        }

        printk(KERN_INFO "Non - Empty ip header"); 
        printk(KERN_INFO "ip_header -> protocol, %u8", ip_header -> protocol); 
        //if we are working with TCP packet
        if(ip_header -> protocol == IPPROTO_TCP) { 
                
                tcp_header= (struct tcphdr *)((__u32 *)ip_header + ip_header -> ihl); 

                sport = htons((unsigned short int) tcp_header -> source); //Getting the source port from the TCP packet header
                dport = htons((unsigned short int) tcp_header -> dest); //Getting the destination port from the TCP packet header
                printk(KERN_INFO "Source port %d", sport); 
                printk(KERN_INFO "Destination port %d", dport); 
        }else if(ip_header -> protocol == IPPROTO_UDP) { //If we are working with the UDP one
                
                
                //udp_header = (struct tcphdr *)skb_transport_header(sock_buff)
                
                udp_header= (struct udphdr *)((__u32 *)ip_header+ ip_header -> ihl); 

                len = htons((unsigned short int) udp_header -> len);
                sport = htons((unsigned short int) udp_header -> source); //Getting the source port from the TCP packet header
                dport = htons((unsigned short int) udp_header -> dest); //Getting the destination port from the TCP packet header
                printk(KERN_INFO "Source port %d", sport); 
                printk(KERN_INFO "Destination port %d", dport); 
        } 
        return 0; 
} 

static void __exit cleanup(void) 
{ 
        printk(KERN_INFO "Cleanup"); 
}

module_init(init);
module_exit(cleanup);

