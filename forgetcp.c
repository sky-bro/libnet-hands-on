#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <libnet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void sendsyn();

char device[100]="wlp3s0";
char err_buf[LIBNET_ERRBUF_SIZE];
libnet_t *lib_net = NULL;
char src_ip_str[16] = "172.20.60.154"; //源主机IP地址
// char *dst_ip_str = "172.20.0.1"; //目的主机IP地址
// 219.217.228.102
char dst_ip_str[16] = "219.217.228.102"; //目的主机IP地址
int dst_port = 80;
int src_port = 12345;
int loopCount = 10;

int main(int argc, char *argv[])
{
  char *cmds = "-i 219.217.228.102 ---- dst_ip\n-p 80 ---- dst_port\n-d wlp3s0 ---- device\n-l 10 ---- loopCount\n-s 0.5 ---- send-syn delay\n";
  char ch;
  int i_len, d_len;
  int s = 1;
	while ((ch = getopt(argc, argv, "i:p:d:l:s:")) != EOF /*-1*/) {
		// printf("optind: %d\n", optind);
   	switch (ch){
				 case 'i':
								 i_len = strlen(optarg)<15?strlen(optarg):15;
								 strncpy(dst_ip_str, optarg, i_len);
								 break;
         case 'p': // 是否采用monitor mode
								 dst_port = atoi(optarg);
								 break;
         case 'd':
                 d_len = strlen(optarg)<99?strlen(optarg):99;
								 strncpy(device, optarg, d_len);
								 break;
         case 'l':
                 loopCount = atoi(optarg);
								 break;
         case 's':
                 s = atof(optarg);
                 if (s<0){
                   s = 1;
                 }
								 break;
				 default:
				 				printf("%s", cmds);
								return 0;
		}
	}
	do
	{
		sendsyn();
		sleep(s);
    loopCount--;
	}while(loopCount>0);

	printf("----ok-----\n");
	return 0;
}

void sendsyn(){
  // int fd = open("./http-content.txt", O_RDONLY);
	char send_msg[1000] = {0};
  // read(fd, send_msg, 1000);
  // close(fd);
  // printf("%s", send_msg);
  int lens = 0;
	libnet_ptag_t lib_t = 0;
	unsigned char src_mac[6] = {0x00,0x0c,0x29,0x97,0xc7,0xc1};//发送者网卡地址00:0c:29:97:c7:c1
  // c8:ff:28:3d:71:fd
  // unsigned char src_mac[6] = {0xc8,0xff,0x28,0x3d,0x71,0xfd};//发送者网卡地址00:0c:29:97:c7:c1
	// unsigned char dst_mac[6] = {0xff,0xff,0xff,0xff,0xff,0xff};//接收者网卡地址‎74-27-EA-B5-FF-D8
  //58:69:6c:a5:e2:d3
  unsigned char dst_mac[6] = {0x58,0x69,0x6c,0xa5,0xe2,0xd3};//接收者网卡地址‎74-27-EA-B5-FF-D8

	unsigned long src_ip,dst_ip = 0;

	// lens = sprintf(send_msg, "%s", "this is for the udp test");
  lens = strlen(send_msg);
  // printf("strlen: %d\n", lens);

 	lib_net = libnet_init(LIBNET_LINK_ADV, device, err_buf);	//初始化
	if(NULL == lib_net)
	{
		perror("libnet_init");
		exit(-1);
	}

	src_ip = libnet_name2addr4(lib_net,src_ip_str,LIBNET_RESOLVE);	//将字符串类型的ip转换为顺序网络字节流
  // srand(time(NULL));
  // src_ip = rand()%2e9;
  // src_ip |= ((rand() % 0xffff)<<16);
	dst_ip = libnet_name2addr4(lib_net,dst_ip_str,LIBNET_RESOLVE);
/*
libnet_ptag_t libnet_build_tcp(u_int16_t sp, u_int16_t dp,u_int32_t seq, u_int32_t ack,u_int8_t control, u_int16_t win,u_int16_t sum, u_int16_t urg,u_int16_t len, u_int8_t *payload,u_int32_t payload_s, libnet_t *l,libnet_ptag_t ptag );
sp：源端口号
dp：目的端口号
seq：序号
ack：ack 标记
control：控制标记
win：窗口大小
sum：校验和，设为 0，libnet 自动填充
urg：紧急指针
len：tcp包长度
payload：负载，为给应用程序发送的文本内容，可设置为 NULL
payload_s：负载长度，或为 0
l：libnet_init() 返回的 libnet * 指针
ptag：协议标记，第一次组新的发送包时，这里写 0，同一个应用程序，下一次再组包时，这个位置的值写此函数的返回值。

返回值：
成功：协议标记
失败：-1
*/
  src_port = rand()%65535;
	lib_t = libnet_build_tcp(	//构造tcp数据包
								src_port, // src_port
								dst_port, // dst_port
                0x6fa13d27&(rand()%0xff), // seq
                0, // ack
                TH_SYN,
                14600,
                0,
                0,
                LIBNET_TCP_H + lens,
                // send_msg,
                NULL,
                // lens,
                0,
								lib_net,
								0
							);
  // if (lib_t == -1) {
  //       printf("libnet_build_tcp failure\n");
  //       return (-3);
  //   };
  src_ip |= ((rand() % 0xffff)<<16);
	lib_t = libnet_build_ipv4(	//构造ip数据包
								40,
								0,
								500,
								0,
								10,
								IPPROTO_TCP,
								0,
								src_ip,
								dst_ip,
								NULL,
								0,
								lib_net,
								0
							);

	lib_t = libnet_build_ethernet(	//构造以太网数据包
									(u_int8_t *)dst_mac,
									(u_int8_t *)src_mac,
									0x800, // 或者，ETHERTYPE_IP
									NULL,
									0,
									lib_net,
									0
								);
	int res = 0;
	res = libnet_write(lib_net);	//发送数据包
  char* tmp = libnet_addr2name4(src_ip, LIBNET_DONT_RESOLVE);
  printf("send from %s:%d to %s:%d\n", tmp, src_port, dst_ip_str, dst_port);
  libnet_destroy(lib_net);	//销毁资源
	if(-1 == res)
	{
		perror("libnet_write");
		exit(-1);
	}
}
