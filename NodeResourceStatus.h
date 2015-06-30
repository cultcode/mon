#ifndef NODERESOURCESTATUS_H
#define NODERESOURCESTATUS_H

#include "common.h"
/* Maximum number of lines in /proc files */
#define PROC_MAXLINES (16*1024)

#define CPUMAX 256

#define P_CPUINFO  0
#define P_STAT    1
#define P_VERSION  2
#define P_MEMINFO     3
#define P_UPTIME     4
#define P_LOADAVG     5
#define P_NFS     6
#define P_NFSD     7
#define P_VMSTAT  8
#define P_NUMBER  9 /* one more than the max */

#undef isdigit
#define isdigit(ch) ( ( (ch) >= '0' &&  (ch) <= '9')? 1: 0 )

#undef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))

#define IFDELTA(member) ((float)( (q->ifnets[i].member > p->ifnets[i].member) ? 0 : (p->ifnets[i].member - q->ifnets[i].member)/elapsed) )
#define IFDELTA_ZERO(member1,member2) ((IFDELTA(member1) == 0) || (IFDELTA(member2)== 0)? 0.0 : IFDELTA(member1)/IFDELTA(member2) )

#define DKDELTA(i,member) ( (q->dk[i].member > p->dk[i].member) ? 0 : (p->dk[i].member - q->dk[i].member))

#define DKDELTAMAX_ZERO(i,j,member) \
if(!p->dk[i].member) {\
  if(DKDELTA(i,member) < DKDELTA(j,member)) {\
    p->dk[i].member = p->dk[j].member;\
    q->dk[i].member = q->dk[j].member;\
  }\
}


#define DISKMAX 256
#define DISK_MODE_DISKSTATS 1
#define DISK_MODE_PARTITIONS 2
#define DISK_MODE_UNKNOWN 3

#define JFSMAX 128
#define LOAD 1
#define UNLOAD 0
#define JFSNAMELEN 64
#define JFSTYPELEN 8

#define CONNECTION_LEN 32

#define CONTENT_LEN 3072
#define CONTENTLEN_LEN 8

#define STATUSDESC_LEN 128

#define PROTOCOL_LEN 16
#define HOST_LEN 64
#define PATH_LEN 256

#define FILE_LINE_BUFFER 1024
#define NODENAME_LEN 128
#define SLAVES_LEN 1024

#define SERVER_VERSION 1

enum {
  FAIL=0,
  SUCCESS=1
};
struct proc{
  int size;
  char *filename;
  FILE *fp;
  int lines;
  char *line[PROC_MAXLINES];
  char *buf;
};

struct cpu_stat {
  unsigned long long user;
  unsigned long long sys;
  unsigned long long wait; 
  unsigned long long idle;
  unsigned long long irq;
  unsigned long long softirq;
  unsigned long long steal;
  unsigned long long nice;
};

struct cpu_stats{
  struct cpu_stat cpu_total;
  struct cpu_stat cpuN[CPUMAX];
  double time;
};

struct cpu_param {
  float total_usage;
  float usage[CPUMAX];
};

struct cpu_data {
  int cpus;
  struct cpu_stats cpu_stats[2];
  struct cpu_stats *p;
  struct cpu_stats *q;
  struct cpu_param cpu_param;
};

//from /etc/mtab
struct jfs {
  char device[JFSNAMELEN];//i.e. /dev/sda1
  char name[JFSNAMELEN];  //i.e. /opt
  char type[JFSNAMELEN];
  int  fd;
  int  mounted;
  float size;
  float bsize;
  float free;
  float usage;
  float io;
  char slaves[SLAVES_LEN];
  int  index;
};

struct dsk_stat { 
  char  dk_name[32];
  int dk_major;
  int dk_minor;
  unsigned long long dk_noinfo;
  unsigned long long dk_reads;
  unsigned long long dk_rmerge;
  unsigned long long dk_rmsec;
  unsigned long long dk_rkb;
  unsigned long long dk_writes;
  unsigned long long dk_wmerge;
  unsigned long long dk_wmsec;
  unsigned long long dk_wkb;
  unsigned long long dk_xfers;
  unsigned long long dk_bsize;
  unsigned long long dk_time;
  unsigned long long dk_inflight;
  unsigned long long dk_11;
  unsigned long long dk_partition;
  unsigned long long dk_blocks; /* in /proc/partitions only */
  unsigned long long dk_use;
  unsigned long long dk_aveq;
};

struct dsk_stats{
  struct dsk_stat dk[DISKMAX];
  double time;
};

struct dsk_data {
  int jfses;
  struct jfs jfs[JFSMAX];
  int disk_mode;
  int disks;
  struct dsk_stats dk[2];
  struct dsk_stats *p;
  struct dsk_stats *q;
};

struct mem_stat {
  unsigned long long memtotal;
  unsigned long long memfree;
  unsigned long long buffers;
  unsigned long long cached;
};

struct mem_stats{
  struct mem_stat  mem_stat;
  double time;
};

struct mem_param {
  float usage;
};

struct mem_data {
  struct mem_stats *p;
  struct mem_stats *q;
  struct mem_stats mem_stats[2];
  struct mem_param mem_param;
};

#define NETMAX 32
struct net_stat {
  char if_name[32];
  unsigned long long if_ibytes;
  unsigned long long if_obytes;
  unsigned long long if_ipackets;
  unsigned long long if_opackets;
  unsigned long long if_ierrs;
  unsigned long long if_oerrs;
  unsigned long long if_idrop;   
  unsigned long long if_ififo;   
  unsigned long long if_iframe;   
  unsigned long long if_odrop;   
  unsigned long long if_ofifo;   
  unsigned long long if_ocarrier;   
  unsigned long long if_ocolls;   
};

struct net_stats {
  struct net_stat ifnets[NETMAX];
  double time;
};

struct net_param {
  char name[32];
  short flags;
  long bandwidth;
  char slaves[SLAVES_LEN];
  char ip[IP_LEN];
  float speed_ib;
  float speed_ob;
  float speed_ip;
  float speed_op;
  float size_i;
  float size_o;
  float peak_i;
  float peak_o;
  float usage;
};

struct net_data {
  int networks;
  struct net_stats net_stats[2];
  struct net_stats *p;
  struct net_stats *q;
  int nets;
  struct net_param net_param[NETMAX];
};

struct con_address {
  unsigned int ip;
  unsigned int port;
};

struct con_stat {
  unsigned long long sl;
  struct con_address local_address;
  struct con_address rem_address;
  unsigned long long st;
  unsigned long long tx_queue;
  unsigned long long rx_queue;
  unsigned long long tr;
  unsigned long long tm_when;
  unsigned long long retrnsmt;
  unsigned long long uid;
  unsigned long long timeout;
  unsigned long long inode;
  unsigned long long ref;
  void* pointer;
  unsigned long long drops;
};

#define SWITCHER(a,b) {\
  typeof(a) _t = (a);\
  (a) = (b);\
  (b) = _t;}

#define DEFAULT_WAYTOGETCONS 1
extern int waytogetcons;


extern double doubletime(void);
extern void proc_init(struct proc *);
extern void GetCpuState(struct cpu_data * data,struct proc * proc);
extern void GetMemState(struct mem_data *data,struct proc * proc);
extern void GetDiskState(struct dsk_data *);
//extern long GetDiskTotalSpace();
//extern long GetDiskFreeSpace();
//extern float GetIoUsage();
extern void GetNetworkState(struct net_data *);
//extern float GetWanUsage();
//extern float GetLanUsage();
//extern int GetWanIpState();
//extern int GetLanIpState();
//extern long GetCurrentBandwidth();
extern unsigned long long GetCurrentConn(char* ip, short port);
#endif
