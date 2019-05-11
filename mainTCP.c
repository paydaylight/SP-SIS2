#include <linux/init.h>            
#include <linux/module.h>           
#include <linux/kernel.h>           

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/fcntl.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/skbuff.h>
#include <linux/if_arp.h>
#include <linux/netdevice.h> 
#include <linux/etherdevice.h>

#include <linux/ip.h> 
#include <linux/udp.h>
#include <linux/tcp.h>

MODULE_LICENSE("GPL");              // Taints Kernel otherwise
 
 
static int send_my(struct net_device* dev, uint8_t dest_addr[ETH_ALEN], uint16_t proto);

static int __init helloWorld_init(void)
{
        uint16_t proto;
        uint8_t dest_addr[ETH_ALEN]; // ETH_ALEN contains number of octets in one ethernet addrwss
        
        static char addr[ETH_ALEN] = {0xff,0xff,0xff,0xff,0xff,0xff};
        struct net_device *wlp7s0;
        
        wlp7s0 = dev_get_by_name(&init_net, "wlp7s0");

        memcpy(dest_addr, addr, ETH_ALEN);
        
        proto = ETH_P_IP;
        send_my(wlp7s0, dest_addr, proto);
        
        printk(KERN_INFO "Module loaded!\n" );
        return 0;
}

unsigned int inet_addr(char *str)
{
        int a, b, c, d;
        char arr[4];

        sscanf(str, "%d.%d.%d.%d", &a, &b, &c, &d);
        arr[0] = a; arr[1] = b; arr[2] = c; arr[3] = d;

        return *(unsigned int *)arr;
}

int send_my(struct net_device* dev, uint8_t dest_addr[ETH_ALEN], uint16_t proto)
{  
        int            ret;
        unsigned char *data;

        char *srcIP = "127.0.0.1";
        char *dstIP = "127.0.0.2";
        char *hello_world = "Hello World";
        int data_len = 11;
	int tcp_header_len = 32;

        int tcp_payload_len = data_len;
        int tcp_total_len = tcp_payload_len + tcp_header_len;

        int ip_header_len = 20;
        int ip_payload_len = tcp_total_len;
        int ip_total_len = ip_header_len + ip_payload_len;

        /* skb */
        struct sk_buff* skb = alloc_skb(ETH_HLEN + ip_total_len, GFP_ATOMIC); //allocate a network buffer
        skb->dev = dev;
        skb->pkt_type = PACKET_OUTGOING;
        skb_reserve(skb, ip_header_len + ETH_HLEN + tcp_header_len);
        
        /* allocate space to data and write it */
        data = skb_put(skb, tcp_payload_len);
        memcpy(data, hello_world, data_len);
        
        /*TCP */
        struct tcphdr* th = (struct tcphdr*)skb_push(skb, tcp_header_len);
        th->source = htons(8080);
        th->dest = htons(3000);
        th->seq = 0x10; //32 bit sequence number, initially set to zero
        th->ack_seq = 0x10; //32 bit ack sequence number, depends whether ACK is set or not
        th->doff = 5; //4 bits: 5 x 32-bit words on tcp header
        th->res1 = 1001; //4 bits: Not used
        th->cwr = 0; //Congestion control mechanism
        th->ece = 0; //Congestion control mechanism
        th->urg = 0; //Urgent flag
        th->ack = 0; //Acknownledge
        th->psh = 0; //Push data immediately
        th->rst = 0; //RST flag
        th->syn = 1; //SYN flag
        th->fin = 0; //Terminates the connection
        th->window = htons(155);//0xFFFF; //16 bit max number of databytes 
        th->check = 0; //16 bit check sum. Can't calculate at this point
        th->urg_ptr = 0; //16 bit indicate the urgent data. Only if URG flag is set

        /* IP header */
        struct iphdr* iph = (struct iphdr*)skb_push(skb,ip_header_len);
        iph->ihl = ip_header_len / 4; 
        iph->version = 4; // IPv4
        iph->tos = 0; 
        iph->tot_len= htons(ip_total_len);  // Total Length
        iph->frag_off = 0; // Fragment offset 
        iph->ttl = 64; // For how long packet will be alive
        iph->protocol = IPPROTO_TCP; //  Implementing TCP
        iph->check = iph->tot_len; 
        iph->saddr = inet_addr(srcIP);
        iph->daddr = inet_addr(dstIP);

        /*changing Mac address */
        struct ethhdr* eth = (struct ethhdr*)skb_push(skb, sizeof(struct ethhdr)); // Add data to the start of a buffer
        skb->protocol = eth->h_proto = htons(proto); // packet type ID field
        skb->no_fcs = 1; // Last 4 bytes of Ethernet FCS
        memcpy(eth->h_source, dev->dev_addr, ETH_ALEN); 
        memcpy(eth->h_dest, dest_addr, ETH_ALEN);


        skb->pkt_type = PACKET_OUTGOING;
        ret = dev_queue_xmit(skb);
        return 1;
} 

static void __exit helloWorld_exit(void){
        printk(KERN_INFO "Module unloaded!\n");
}
 
module_init(helloWorld_init);
module_exit(helloWorld_exit);

