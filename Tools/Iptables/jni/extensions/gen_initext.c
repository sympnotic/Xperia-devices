
extern void ipt_ah_init(void);
extern void ipt_addrtype_init(void);
extern void ipt_comment_init(void);
extern void ipt_2connmark_init(void);
extern void ipt_conntrack_init(void);
extern void ipt_2dscp_init(void);
extern void ipt_2ecn_init(void);
extern void ipt_esp_init(void);
extern void ipt_hashlimit_init(void);
extern void ipt_helper_init(void);
extern void ipt_icmp_init(void);
extern void ipt_ipt_iprange_init(void);
extern void ipt_length_init(void);
extern void ipt_limit_init(void);
extern void ipt_mac_init(void);
extern void ipt_multiport_init(void);
extern void ipt_2mark_init(void);
extern void ipt_owner_init(void);
extern void ipt_physdev_init(void);
extern void ipt_pkttype_init(void);
extern void ipt_policy_init(void);
extern void ipt_realm_init(void);
extern void ipt_sctp_init(void);
extern void ipt_standard_init(void);
extern void ipt_state_init(void);
extern void ipt_tcp_init(void);
extern void ipt_2tcpmss_init(void);
extern void ipt_2tos_init(void);
extern void ipt_2ttl_init(void);
extern void ipt_udp_init(void);
extern void ipt_unclean_init(void);
extern void ipt_CLASSIFY_init(void);
extern void ipt_CONNMARK_init(void);
extern void ipt_DNAT_init(void);
extern void ipt_LOG_init(void);
//extern void ipt_DSCP_init(void);
//extern void ipt_ECN_init(void);
extern void ipt_MASQUERADE_init(void);
extern void ipt_MIRROR_init(void);
extern void ipt_NETMAP_init(void);
extern void ipt_NFQUEUE_init(void);
extern void ipt_NOTRACK_init(void);
extern void ipt_REDIRECT_init(void);
extern void ipt_REJECT_init(void);
extern void ipt_MARK_init(void);
void init_extensions(void) {
	ipt_ah_init();
	ipt_addrtype_init();
	ipt_comment_init();
	ipt_2connmark_init();
	ipt_conntrack_init();
	ipt_2dscp_init();
	ipt_2ecn_init();
	ipt_esp_init();
	ipt_hashlimit_init();
	ipt_helper_init();
	ipt_icmp_init();
	ipt_iprange_init();
	ipt_length_init();
	ipt_limit_init();
	ipt_mac_init();
	ipt_multiport_init();
	ipt_2mark_init();
	ipt_owner_init();
	ipt_physdev_init();
	ipt_pkttype_init();
	ipt_policy_init();
	ipt_realm_init();
	ipt_sctp_init();
	ipt_standard_init();
	ipt_state_init();
	ipt_tcp_init();
	ipt_2tcpmss_init();
	ipt_2tos_init();
	ipt_2ttl_init();
	ipt_udp_init();
	ipt_unclean_init();
	ipt_CLASSIFY_init();
	ipt_CONNMARK_init();
	ipt_DNAT_init();
	ipt_LOG_init();
//	ipt_DSCP_init();
//	ipt_ECN_init();
	ipt_MASQUERADE_init();
	ipt_MIRROR_init();
	ipt_NETMAP_init();
	ipt_NFQUEUE_init();
	ipt_NOTRACK_init();
	ipt_REDIRECT_init();
	ipt_REJECT_init();
	ipt_MARK_init();
}
