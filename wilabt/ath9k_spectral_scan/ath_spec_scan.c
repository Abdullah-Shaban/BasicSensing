/*
 * wifi_spectrum_scan
 *
 * This application is a wrapper for the atheros Wi-Fi chipsets (i.e. AR92xx and AR93xx) which collects Wi-Fi spectral samples and store
 * the information into a database. The information stored is pre-processed twice, first by calculating the energies present in the Wi-Fi
 * and zigbee channels and second by reducing the amount of information using different holding methods (i.e. MAX_HOLD, MIN_HOLD and AVG_HOLD)
 *
 * Wi-Fi and Zigbee 2.4 GhZ ISM channel overlap
   --------------------------------------------
		   ┌──────────────────────────────────┐	   ┌──────────────────────────────────┐	   ┌──────────────────────────────────┐
Wi-Fi Channels	   │		     1 		      │	   │		      6		      │    │		     11		      │
		   └───────┬──────────────────────────┴────┴──┬────┬──────────────────────────┴────┴──┬────┬──────────────────────────┴───────┐
		   	   │		     2 		      │	   │		      7		      │    │		     12		      │
		   	   └───────┬──────────────────────────┴────┴──┬────┬──────────────────────────┴────┴──┬────┬──────────────────────────┴───────┐
		   	   	   │		     3 		      │	   │		      8		      │    │		     13		      │
		   	   	   └───────┬──────────────────────────┴────┴──┬────┬──────────────────────────┴────┴──┬───────────────────────────────┘
		   	   		   │		     4 		      │	   │		      9		      │
		   	   		   └───────┬──────────────────────────┴────┴──┬────┬──────────────────────────┴───────┐
		   	   			   │		     5		      │	   │		     10		      │
		   	   			   └──────────────────────────────────┘	   └──────────────────────────────────┘
		       ┌──┐    ┌──┐    ┌──┐    ┌──┐    ┌──┐    ┌──┐    ┌──┐    ┌──┐    ┌──┐    ┌──┐    ┌──┐    ┌──┐    ┌──┐    ┌──┐    ┌──┐    ┌──┐
Zigbee Channels        │11│    │12│    │13│    │14│    │15│    │16│    │17│    │18│    │19│    │20│    │21│    │22│    │23│    │24│    │25│    │26│
		       └──┘    └──┘    └──┘    └──┘    └──┘    └──┘    └──┘    └──┘    └──┘    └──┘    └──┘    └──┘    └──┘    └──┘    └──┘    └──┘
	     |     
	     |     
Freq [MhZ] --|---------------------------------------------------------------------------------------------------------------------------------------------->
    2400 +   |      02  05  07  10  12  15  17  20  22  25  27  30  32  35  37  40  42  45  47  50  52  55  57  60  62  65  67  70  72  75  77  80  82  85


Author:	Michael T. Mehari
Email:	michael.mehari@intec.ugent.be
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include <stdlib.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <time.h>

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include <linux/nl80211.h>

// MACRO DEFINITION
#define BOLD   				"\033[1m\033[30m"
#define UL				"\033[4m"
#define RESET   			"\033[0m"
#define CLEAR_LINE			"%c[2K"

#define PACKED				__attribute__((packed))
#define SPECTRAL_HT20_NUM_BINS		56
#define SUB_BUF_SIZE			1024
#define SAMPLE_SIZE_PER_SUB_BUF		(SUB_BUF_SIZE/sizeof(struct fft_sample_ht20))*sizeof(struct fft_sample_ht20)

#define	ATH_FFT_SAMPLE_HT20		1
#define	ATH_FFT_SAMPLE_HT40		2

char 	ieee80211_debug_path[] 	= "/sys/kernel/debug/ieee80211";
char 	ieee80211_class_path[] 	= "/sys/class/ieee80211";
char 	rx_spectral_name[]	= "RX-Spectral";
char	*phy_name		= "phy0";
char	IF_name[IFNAMSIZ]	= "";

int finished = 0;

/* Packet statistics enable/disable flag enumeration */
enum pkt_stat_t
{
	DISABLED = 0,
	ENABLED = 1
};

struct fft_sample_tlv
{
	uint8_t type; 
	uint16_t length;
} PACKED;

struct fft_sample_ht20
{
	struct fft_sample_tlv tlv;

	uint8_t max_exp;
	uint16_t freq;
	int8_t rssi;
	int8_t noise;
	uint16_t max_magnitude;
	uint8_t max_index;
	uint8_t bitmap_weight;
	uint64_t tsf;
	uint8_t data[SPECTRAL_HT20_NUM_BINS];
} PACKED;

struct nl80211_state {
	struct nl_sock *nl_sock;
	int nl80211_id;
};

// Get the mac address of the specifed physical interface
void ret_phyIF_MAC(char *phy_name, char *mac_str)
{
	char command[128];
	FILE *fp;

	sprintf(command,"cat %s/%s/macaddress", ieee80211_class_path, phy_name);
	fp = popen(command, "r");
	if(fp != NULL)
		fgets(mac_str, 18, fp);
	pclose(fp);
}

// Error callback handler
static int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg)
{
	int *ret = arg;
	*ret = err->error;
	return NL_STOP;
}

// Finish callback handler
static int finish_handler(struct nl_msg *msg, void *arg)
{
	int *ret = arg;
	*ret = 0;
	return NL_SKIP;
}

// Acknowledgment callback handler
static int ack_handler(struct nl_msg *msg, void *arg)
{
	int *ret = arg;
	*ret = 0;
	return NL_STOP;
}

// valid interface callback handler
static int iface_handler(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];

	nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);

	if (tb_msg[NL80211_ATTR_WIPHY])
	{
		char wiphy[IFNAMSIZ];
		sprintf(wiphy, "phy%d", nla_get_u32(tb_msg[NL80211_ATTR_WIPHY]));

		// If selected physical interface matches the result, search for a monitor interface and copy the name
		if(strcmp(phy_name, wiphy) == 0)
		{
			if (tb_msg[NL80211_ATTR_IFTYPE] && tb_msg[NL80211_ATTR_IFNAME])
			{
				// If the interface type is monitor
				if(nla_get_u32(tb_msg[NL80211_ATTR_IFTYPE]) == 6)
					strcpy(IF_name, nla_get_string(tb_msg[NL80211_ATTR_IFNAME]));
			}
		}
	}

	return NL_SKIP;
}

// Retrieve monitor interface for a given physical interface
int ret_mon_IF(struct nl80211_state *state)
{
	struct nl_msg *msg;
	struct nl_cb *cb, *s_cb;
	int err = 1;

	msg = nlmsg_alloc();
	if (!msg)
	{
		fprintf(stderr, "failed to allocate netlink message\n");
		return 2;
	}

	// Allocate a new callback handle.
	cb = nl_cb_alloc(NL_CB_DEFAULT);
	s_cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!cb || !s_cb)
	{
		fprintf(stderr, "failed to allocate netlink callbacks\n");
		err = 2;
		goto out_free_msg;
	}

	// Add Generic Netlink header (i.e. NL80211_CMD_GET_INTERFACE) to Netlink message
	genlmsg_put(msg, 0, 0, state->nl80211_id, 0, 768, NL80211_CMD_GET_INTERFACE, 0);

	// Set up a valid callback function
	if (nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, iface_handler, NULL))
		goto out;

	// Attach the callback function to a netlink socket
	nl_socket_set_cb(state->nl_sock, s_cb);

	// Finalize and transmit Netlink message
	if(nl_send_auto_complete(state->nl_sock, msg) < 0)
		goto out;

	// Set up error, finish and acknowledgment callback function
	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);

	while (err > 0)
		nl_recvmsgs(state->nl_sock, cb);

out:
	nl_cb_put(cb);
out_free_msg:
	nlmsg_free(msg);
	return err;
}

// Initialize netlink socket
static int nl80211_init(struct nl80211_state *state)
{
	int err;

	state->nl_sock = nl_socket_alloc();
	if (!state->nl_sock)
	{
		fprintf(stderr, "Failed to allocate netlink socket.\n");
		return -ENOMEM;
	}

	if (genl_connect(state->nl_sock))
	{
		fprintf(stderr, "Failed to connect to generic netlink.\n");
		err = -ENOLINK;
		goto out_handle_destroy;
	}

	state->nl80211_id = genl_ctrl_resolve(state->nl_sock, "nl80211");
	if (state->nl80211_id < 0)
	{
		fprintf(stderr, "nl80211 not found.\n");
		err = -ENOENT;
		goto out_handle_destroy;
	}

	return 0;

out_handle_destroy:
	nl_socket_free(state->nl_sock);
	return err;
}

// Reset Monitor mode interface by bringing it down and up
int reset_mon_IF()
{
	int sockfd, err;
	struct ifreq ifr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockfd < 0)
		return sockfd;

	memset(&ifr, 0, sizeof ifr);

	strncpy(ifr.ifr_name, IF_name, IFNAMSIZ);

	// First bring the interface down
	ifr.ifr_flags = ifr.ifr_flags & ~IFF_UP;
	err = ioctl(sockfd, SIOCSIFFLAGS, &ifr);
	if(err < 0)
		return err;

	// Next bring the interface up
	ifr.ifr_flags = ifr.ifr_flags | IFF_UP;
	err = ioctl(sockfd, SIOCSIFFLAGS, &ifr);
	return err;
}

// cleanup resources upon SIGINT and SIGTERM
void cleanup()
{
	finished = 1;
}

int main(int argc, char *argv[])
{
	uint8_t fft_period = 15;
	uint8_t period = 1;
	double noise_thld = -95;
	uint16_t intval = 100;

	int option = -1;
	while ((option = getopt (argc, argv, "hp:f:P:t:i:")) != -1)
	{
		switch (option)
		{
			case 'h':
				fprintf(stderr, "Description\n");
				fprintf(stderr, "-----------\n");
				fprintf(stderr, BOLD "COR" RESET " is a wrapper for the atheros Wi-Fi chipsets (i.e. AR92xx and AR93xx) which collects Wi-Fi spectral samples and store\n");
				fprintf(stderr, "the information into a database. The information stored is pre-processed twice, first by calculating the energies present in the Wi-Fi\n");
				fprintf(stderr, "and zigbee channels and second by reducing the amount of information using different holding methods such as\n");
				fprintf(stderr, "MAX_HOLD, MIN_HOLD and AVG_HOLD for a specific amount of time.\n\n");
				fprintf(stderr, "Argument list\n");
				fprintf(stderr, "-------------\n");
				fprintf(stderr, "-h\t\t\thelp menu\n");
				fprintf(stderr, "-p phy_name\t\tphysical interface name of the wireless card [eg. -p phy0]\n");
				fprintf(stderr, "-f fft_period\t\twhen active and triggered, PHY passes FFT frames to MAC every (fft_period+1)*4uS [eg. -f 15]\n");
				fprintf(stderr, "-P period\t\ttime period between successive spectral scan entry points (period*256*Tclk) [eg. -P 1]\n");
				fprintf(stderr, "-t noise_thld\t\tWi-Fi noise threshold (dBm) for COR calculation [eg. -t -95]\n");
				fprintf(stderr, "-i intval\t\tinterval (msec) of a periodic report [eg. -i 100]\n\n");
				fprintf(stderr, "Example\n");
				fprintf(stderr, "-------\n");
				fprintf(stderr, BOLD "./COR -n 10.11.31.5 -u CREW_BM -k CREW_BM -d benchmarking -p phy0 -f 15 -P 255 -t -95 -i 100\n" RESET);
				return 1;
			case 'p':
				phy_name = strdup(optarg);
				break;
			case 'f':
				fft_period = atoi(optarg);
				break;
			case 'P':
				period = atoi(optarg);
				break;
			case 't':
				noise_thld = atof(optarg);
				break;
			case 'i':
				intval = atoi(optarg);
				break;
			default:
				fprintf(stderr, "COR: missing operand. Type './COR -h' for more information\n");
				return 1;
		}
	}

	int    fd;

	// User space and kernel space scan result counter
	char command[128];
	uint32_t scan_count_temp, COR_count;

	struct timeval systime;
	uint64_t start_time, current_time;
	uint16_t previous_dur, current_dur;


	char subbuf[SUB_BUF_SIZE];
	struct fft_sample_ht20 *sample;
	uint16_t freq;

	int i, j, datasquaresum, num_of_read;
	struct pollfd pfds;

	struct nl80211_state nlstate;
	char sniffer_mac[18];

	// Calculate bin threshold from noise threshold
	double bin_thld = 10*log10f(pow(10,noise_thld/10)/SPECTRAL_HT20_NUM_BINS);

	// handle SIGTERM and SIGINT
	signal(SIGINT, cleanup);
	signal(SIGTERM, cleanup);

	// Initialize netlink handler
	if (nl80211_init(&nlstate))
		return 1;

	// Make sure there exists a monitor mode interface with in the given physical interface
	if(ret_mon_IF(&nlstate) > 0)
		return 1;

	if(IF_name[0] == '\0')
	{
		fprintf(stderr, "%s does not expose a monitor interface. Create one before running spectrum scanning.\n", phy_name);
		fprintf(stderr, "E.g. sudo iw phy %s interface add wlanMoni type monitor && sudo ifconfig wlanMoni up\n", phy_name);
		return 1;
	}

	// Retrieve mac address
	ret_phyIF_MAC(phy_name, sniffer_mac);

	// Log the starting time
	gettimeofday(&systime, NULL);
	start_time = 1e3*systime.tv_sec + (int)(systime.tv_usec/1e3);
	previous_dur = (start_time % intval);

	/* Open CPU-0 spectral relay file */
	sprintf(command,"%s/%s/ath9k/spectral_scan0", ieee80211_debug_path, phy_name);
	fd = open(command, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "open failure for file spectral_scan0: %s\n", strerror(errno));
		return 1;
	}
	// Polling file descriptor and input event request
	pfds.fd = fd;
	pfds.events = POLLIN;

	// Initialize spectral parameters
	sprintf(command,"echo %u > %s/%s/ath9k/spectral_fft_period", fft_period, ieee80211_debug_path, phy_name);
	if(system(command) == -1)
		return 1;
	sprintf(command,"echo %u > %s/%s/ath9k/spectral_period", period, ieee80211_debug_path, phy_name);
	if(system(command) == -1)
		return 1;

	// Enable background spectral scan
	sprintf(command,"echo background > %s/%s/ath9k/spectral_scan_ctl", ieee80211_debug_path, phy_name);
	if(system(command) == -1)
		return 1;

	// Trigger the spectral scan
	sprintf(command,"echo trigger > %s/%s/ath9k/spectral_scan_ctl", ieee80211_debug_path, phy_name);
	if(system(command) == -1)
		return 1;

	// Initialize the user space scan result counter
	scan_count_temp = 0;
	COR_count = 0;

	while(1)
	{
		if(finished == 1)
			break;

		// Before timeout (100 msec) expires, wait the sub buffer to become full. If not, restart the interface and trigger the spectral scan.
		poll(&pfds, 1, 100);
		if(!(pfds.revents & POLLIN))
		{
			// Reset monitor mode interface
			if(reset_mon_IF() < 0)
			{
				fprintf(stderr, "Monitor interface %s reset error.\n", IF_name);
				break;
			}

			// Trigger background spectral scan once again
			sprintf(command,"echo trigger > %s/%s/ath9k/spectral_scan_ctl", ieee80211_debug_path, phy_name);
			if(system(command) == -1)
				break;

			continue;
		}
		else
		{
			num_of_read = read(fd, subbuf, SAMPLE_SIZE_PER_SUB_BUF);
			if(num_of_read < 0)
			{
				fprintf(stderr, "spectral_scan0 read error: %s\n", strerror(errno));
				break;
			}
			else if(num_of_read < sizeof(struct fft_sample_ht20))
			{
				continue;
			}
			else
			{
				// Iterate through the buffer and parse the spectrum data
				for (i = 0; i < num_of_read/sizeof(struct fft_sample_ht20); i++)
				{
					sample = (struct fft_sample_ht20 *)(subbuf + i*sizeof(struct fft_sample_ht20));

					// If sample does not contain a spectrum data
					if((sample->tlv.type != ATH_FFT_SAMPLE_HT20) || (be16toh(sample->tlv.length) != sizeof(struct fft_sample_ht20)-sizeof(struct fft_sample_tlv)))
						continue;

					// Determine spectral frequency
					if(i == 0)
					{
						freq = sample->freq;
					}
					// If spectral frequency changes, reset COR counters
					else if(freq != sample->freq)
					{
						scan_count_temp = 0;
						COR_count = 0;
					}

					/* calculate the data square sum */
					datasquaresum = 0;
					for (j = 0; j < SPECTRAL_HT20_NUM_BINS; j++)
						datasquaresum += (sample->data[j] << sample->max_exp) * (sample->data[j] << sample->max_exp);

					// Check for empty result
					if(datasquaresum == 0)
						continue;

					// Increment the scan result temporary counter
					scan_count_temp++;

					// Calculate COR per each fft bin
					for (j = 0; j < SPECTRAL_HT20_NUM_BINS; j++)
					{
						double signal;
						int data;

						data = sample->data[j] << sample->max_exp;
						if (data == 0)
							data = 1;

						// Signal calculation (i.e. http://wireless.kernel.org/en/users/Drivers/ath9k/spectral_scan/)
						signal = sample->noise + sample->rssi + 20*log10f(data) - 10*log10f(datasquaresum);

						// If instantaneous bin energy is above noise threshold, channel is occupied
						if(signal > bin_thld)
							COR_count++;
					}
				}
			}
		}

		// Get the current time
		gettimeofday(&systime, NULL);
		current_time = 1e3*systime.tv_sec + (int)(systime.tv_usec/1e3);
		current_dur = (current_time % intval);
		if(current_dur < previous_dur)
		{
			// Generate the query statement
			printf("%lu\t%s\t%d\t%.5f\t%u\n", current_time, sniffer_mac, ntohs(sample->freq), (100.0*COR_count)/(scan_count_temp*SPECTRAL_HT20_NUM_BINS), previous_dur);

			// Update parameters for next round
			scan_count_temp = 0;
			COR_count = 0;
		}

		// Update parameter for next round
		previous_dur = current_dur;
	}

	// Disable spectrum scanning
	sprintf(command,"echo disable > %s/%s/ath9k/spectral_scan_ctl", ieee80211_debug_path, phy_name);
	if(system(command) == -1)
		return 1;

	/* Close the spectral scan relay file */
	close(fd);

	return 1;
}
