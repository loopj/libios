#pragma once

#include <stdint.h>

// Device KD, SSL and games open for sockets
#define SO_DEVICE "/dev/net/ip/top"

// SO's receive side, where a link driver delivers frames and the link state
#define SO_BOTTOM_DEVICE "/dev/net/ip/bottom"

// The wireless link driver SO opens for interface type 1, served by NCD
#define SO_WIRELESS_DRIVER_DEVICE "/dev/net/wd/top"

// The wired link driver SO opens for any other interface type, served by ETH
#define SO_WIRED_DRIVER_DEVICE "/dev/net/usbeth/top"

// Sockets open at once, numbered 0 to 23
#define SO_SOCKETS_MAX 24

// Most entries one POLL takes
#define SO_POLL_ENTRIES_MAX 24

// POLL timeout that waits forever
#define SO_POLL_FOREVER 0xffffffffffffffffull

// Longest host name a lookup takes, its terminator included
#define SO_HOSTNAME_MAX 0x100

// Most addresses a lookup returns
#define SO_LOOKUP_ADDRESSES_MAX 35

// Version word that opens so_startup_config
#define SO_STARTUP_VERSION 0x100

// ICMP sockets open at once
#define SO_ICMP_SLOTS 4

// Largest ICMP echo payload
#define SO_ICMP_PAYLOAD_MAX 0xffe4

// Nonblocking flag a FCNTL reads and sets
#define SO_O_NONBLOCK (1 << 2)

// Level of the socket's own options
#define SO_SOL_SOCKET 0xffff

// Level of the TCP options
#define SO_IPPROTO_TCP 6

// Level of every interface option
#define SO_INTERFACE_OPTION_LEVEL 0xfffe

// Routes the route table holds, the answer to SO_INTERFACE_OPTION_ROUTE_CAPACITY
#define SO_ROUTE_CAPACITY 0x20

// Requests, ioctl unless marked. Words are big endian u32 and an address is an 8 byte struct so_sockaddr_in
enum so_request {
  SO_REQUEST_ACCEPT               = 1,    // Accepts a connection, writes the peer's address and returns the new socket
  SO_REQUEST_BIND                 = 2,    // Binds the socket to the address that follows it
  SO_REQUEST_CLOSE                = 3,    // Closes the socket
  SO_REQUEST_CONNECT              = 4,    // Connects the socket to the address that follows it
  SO_REQUEST_FCNTL                = 5,    // Gets or sets the socket's flags, returning them for SO_F_GETFL
  SO_REQUEST_GETPEERNAME          = 6,    // Writes the peer's address
  SO_REQUEST_GETSOCKNAME          = 7,    // Writes the socket's own address
  SO_REQUEST_GETSOCKOPT           = 8,    // Reads a socket option, with its arguments and value both in the output
  SO_REQUEST_SETSOCKOPT           = 9,    // Sets a socket option from the level, option, length and value
  SO_REQUEST_LISTEN               = 10,   // Listens with the backlog, raised to 1 when lower
  SO_REQUEST_POLL                 = 11,   // Polls the entries until one is ready or the timeout in milliseconds passes
  SO_REQUEST_RECVFROM             = 12,   // Receives data and the sender's address, over ioctlv
  SO_REQUEST_SENDTO               = 13,   // Sends data to the socket's peer or an address given, over ioctlv
  SO_REQUEST_SHUTDOWN             = 14,   // Shuts down one or both directions
  SO_REQUEST_SOCKET               = 15,   // Opens a socket of the family, type and protocol 0, returns it
  SO_REQUEST_GETHOSTID            = 16,   // Returns the interface's address
  SO_REQUEST_GETHOSTBYNAME        = 17,   // Looks up a name, writes a struct so_hostent
  SO_REQUEST_GETHOSTBYADDR        = 18,   // Looks up an address, writes a struct so_hostent
  SO_REQUEST_GETNAMEINFO          = 19,   // Writes the host and service text for an address, over ioctlv
  SO_REQUEST_GETLASTERROR         = 20,   // Returns the last error of the worker that takes the request
  SO_REQUEST_INET_ATON            = 21,   // Parses dotted text into an address, returns 1 when it parsed
  SO_REQUEST_INET_PTON            = 22,   // Parses text into an address, returns 1 when it parsed
  SO_REQUEST_INET_NTOP            = 23,   // Writes an address as text
  SO_REQUEST_GETADDRINFO          = 24,   // Looks up a name, writes a struct so_addrinfo_output, over ioctlv
  SO_REQUEST_SOCKATMARK           = 25,   // Returns 1 at the urgent mark
  SO_REQUEST_STARTUP_CONFIG       = 26,   // Starts the stack with a struct so_startup_config
  SO_REQUEST_CLEANUP              = 27,   // Closes every socket and stops the stack
  SO_REQUEST_GET_INTERFACE_OPTION = 28,   // Reads an interface option on level 0xfffe, over ioctlv
  SO_REQUEST_SET_INTERFACE_OPTION = 29,   // Writes an interface option on level 0xfffe, over ioctlv
  SO_REQUEST_SET_INTERFACE        = 30,   // Selects the interface, 1 for wireless and any other value wired
  SO_REQUEST_STARTUP              = 31,   // Brings the interface up without starting the stack
  SO_REQUEST_ICMP_SOCKET          = 0x30, // Opens an ICMP socket of the family, returns its slot
  SO_REQUEST_ICMP_PING            = 0x31, // Pings with a struct so_icmp_ping_input and the payload, over ioctlv
  SO_REQUEST_ICMP_CANCEL          = 0x32, // Ends the slot's ping with SO_EINTR
  SO_REQUEST_ICMP_CLOSE           = 0x33, // Closes the slot
};

// Requests a link driver sends on SO_BOTTOM_DEVICE, both ioctlv
enum so_bottom_request {
  SO_BOTTOM_RECEIVE_FRAME = 0x10001, // Delivers a received Ethernet frame of 14 or more bytes in an iobuf
  SO_BOTTOM_LINK_STATE    = 0x10002, // Delivers the link state, 1 for up and 2 for down
};

// Requests SO sends on a link driver device, which answers -10 for a link that is not up and -4 for any other
enum so_driver_request {
  SO_DRIVER_TRANSMIT  = 0x20001, // Sends the Ethernet frame in an iobuf
  SO_DRIVER_LINK_UP   = 0x20002, // Brings the link up within 30 seconds, which SO reports as SO_ENOLINK on failure
  SO_DRIVER_LINK_DOWN = 0x20003, // Takes the link down
  SO_DRIVER_SET_MAC   = 0x20005, // Sets the 6 byte MAC address
  SO_DRIVER_GET_MAC   = 0x20006, // Writes the 6 byte MAC address
};

// Results, a count or one of these
enum so_result {
  SO_OK              = 0,
  SO_EADDRINUSE      = -3,
  SO_EFAIL           = -4, // A malformed request
  SO_EAFNOSUPPORT    = -5,
  SO_EAGAIN          = -6,
  SO_EBADF           = -8,
  SO_ECONNABORTED    = -13,
  SO_ECONNRESET      = -15,
  SO_EDESTADDRREQ    = -17,
  SO_EINPROGRESS     = -26,
  SO_EINTR           = -27,
  SO_EINVAL          = -28,
  SO_EMFILE          = -33,
  SO_EMSGSIZE        = -35,
  SO_ENETDOWN        = -38,
  SO_ENETRESET       = -39, // The stack is not started
  SO_ENETUNREACH     = -40,
  SO_ENOBUFS         = -42,
  SO_ENOLINK         = -48,
  SO_ENOMEM          = -49,
  SO_ENOPROTOOPT     = -51,
  SO_ENOTCONN        = -56,
  SO_ENXIO           = -62,
  SO_EOPNOTSUPP      = -63,
  SO_EPROTONOSUPPORT = -68,
  SO_EPROTOTYPE      = -69,
  SO_ETIMEDOUT       = -76,
  SO_EAI_FAIL        = -302, // The lookup failed
  SO_EAI_FAMILY      = -303, // A bad family, or no name
  SO_EAI_MEMORY      = -304,
  SO_EAI_NONAME      = -305, // No result
};

// Address families
enum so_family {
  SO_AF_INET  = 2,
  SO_AF_INET6 = 23,
};

// Socket types
enum so_socket_type {
  SO_SOCK_STREAM = 1,
  SO_SOCK_DGRAM  = 2,
};

// RECVFROM and SENDTO flags
enum so_message_flag {
  SO_MSG_OOB      = (1 << 0),
  SO_MSG_PEEK     = (1 << 1),
  SO_MSG_DONTWAIT = (1 << 2),
};

// POLL events, of which the last three are only returned
enum so_poll_event {
  SO_POLLRDNORM = (1 << 0),
  SO_POLLRDBAND = (1 << 1),
  SO_POLLPRI    = (1 << 2),
  SO_POLLWRNORM = (1 << 3),
  SO_POLLWRBAND = (1 << 4),
  SO_POLLERR    = (1 << 5),
  SO_POLLHUP    = (1 << 6),
  SO_POLLNVAL   = (1 << 7),
};

// FCNTL commands
enum so_fcntl_command {
  SO_F_GETFL = 3,
  SO_F_SETFL = 4,
};

// Socket options on SO_SOL_SOCKET, each a u32 unless marked
enum so_socket_option {
  SO_SO_REUSEADDR = 0x0004,
  SO_SO_LINGER    = 0x0080, // An on flag, then the time in seconds
  SO_SO_OOBINLINE = 0x0100,
  SO_SO_SNDBUF    = 0x1001, // Rounded up to 0x1000, 0x2000, 0x4000 or 0x8000, refused above
  SO_SO_RCVBUF    = 0x1002, // Rounded up to 0x1000, 0x2000, 0x4000 or 0x8000, refused above
  SO_SO_SNDLOWAT  = 0x1003,
  SO_SO_RCVLOWAT  = 0x1004,
  SO_SO_TYPE      = 0x1008, // Read only
  SO_SO_ERROR     = 0x1009, // Read only, the pending error
};

// TCP options on SO_IPPROTO_TCP, each a u32
enum so_tcp_option {
  SO_TCP_NODELAY     = 0x2001, // Turns off Nagle's algorithm
  SO_TCP_MAXSEG      = 0x2002,
  SO_TCP_URGENT_LAST = 0x2003, // The urgent pointer marks the last urgent byte, not the one after
  SO_TCP_R2          = 0x2004, // Seconds without progress before dropping a connection
  SO_TCP_R2_SYN      = 0x2005, // The same while the handshake is unanswered, 180 by default
};

// GETNAMEINFO flags
enum so_nameinfo_flag {
  SO_NI_NOFQDN      = (1 << 0),
  SO_NI_NUMERICHOST = (1 << 1),
  SO_NI_NAMEREQD    = (1 << 2),
};

// Interface options on level 0xfffe
enum so_interface_option {
  SO_INTERFACE_OPTION_DHCP_STATE     = 0x1003, // DHCP error state, u32
  SO_INTERFACE_OPTION_MAC            = 0x1004, // MAC address, 6 bytes
  SO_INTERFACE_OPTION_LINK_UP        = 0x1005, // 1 while the link is up, u32
  SO_INTERFACE_OPTION_FAMILY         = 0x4002, // The constant 2, u32
  SO_INTERFACE_OPTION_ADDRESS        = 0x4003, // Address, netmask and broadcast, 12 bytes
  SO_INTERFACE_OPTION_ROUTE_CAPACITY = 0x4005, // SO_ROUTE_CAPACITY, u32
  SO_INTERFACE_OPTION_ROUTES         = 0x4006, // A struct so_route for each route that fits
  SO_INTERFACE_OPTION_LINK_STATE     = 0xa001, // Link state, u32
  SO_INTERFACE_OPTION_HOST_NAME      = 0xb001, // Host name, up to 0x100 bytes
  SO_INTERFACE_OPTION_DNS_CAPACITY   = 0xb002, // The constant 2, the most servers SO_INTERFACE_OPTION_DNS_SERVERS holds
  SO_INTERFACE_OPTION_DNS_SERVERS    = 0xb003, // Up to two DNS servers, set as 4 or 8 bytes
  SO_INTERFACE_OPTION_LEASE_TIME     = 0xc001, // Remaining DHCP lease in seconds, u32
};

// Route flags in so_route
enum so_route_flag {
  SO_ROUTE_GATEWAY = (1 << 0), // Goes through its gateway
};

// Startup flags in so_startup_config
enum so_startup_flag {
  SO_STARTUP_DHCP                 = (1 << 0),
  SO_STARTUP_PPPOE                = (1 << 1),
  SO_STARTUP_IPV6                 = (1 << 2),
  SO_STARTUP_DHCP_REQUEST_ADDRESS = (1 << 8),  // DHCP asks for the static address
  SO_STARTUP_LINK_LOCAL           = (1 << 15), // Assign a 169.254 address
};

// An IPv4 socket address, 8 bytes, whose length byte an output must preset to at least 8
struct so_sockaddr_in {
  uint8_t len;
  uint8_t family;
  uint16_t port;
  uint32_t address;
};

// An IPv6 socket address, 0x1c bytes, which only the text requests take
struct so_sockaddr_in6 {
  uint8_t len;
  uint8_t family;
  uint16_t port;

  // Assumed flow info, which fits the size and the address offset
  uint32_t flow_info;

  uint8_t address[16];

  // Assumed scope id, which fits the size
  uint32_t scope_id;
};

// The hostent header, its pointers IOS addresses that a caller rebases from name minus 0x10
struct so_hostent_header {
  uint32_t name;

  // An empty list, always
  uint32_t aliases;

  uint16_t address_type;
  uint16_t address_len;
  uint32_t address_list;
};

// What a name lookup writes back, 0x460 bytes
struct so_hostent {
  struct so_hostent_header header;
  char name[SO_HOSTNAME_MAX];
  uint32_t addresses[0x230 / 4];

  // Pointers to the addresses, ended by 0
  uint32_t address_list[0x120 / 4];
};

// One addrinfo node, 0x20 bytes
struct so_addrinfo {
  uint32_t flags;
  uint32_t family;
  uint32_t socket_type;
  uint32_t protocol;
  uint32_t address_len;
  uint32_t canonical_name;
  uint32_t address;

  // 0 on the last node
  uint32_t next;
};

// One packed address after the addrinfo nodes, 0x1c bytes, the size of an IPv6 address
struct so_addrinfo_address {
  struct so_sockaddr_in address;
  uint8_t padding[0x14];
};

// What GETADDRINFO writes back, the address of node i at addresses[i] since node pointers stay IOS addresses
struct so_addrinfo_output {
  struct so_addrinfo nodes[SO_LOOKUP_ADDRESSES_MAX];
  struct so_addrinfo_address addresses[SO_LOOKUP_ADDRESSES_MAX];
};

// A BIND or CONNECT input
struct so_address_input {
  uint32_t socket;

  // Always 1
  uint32_t has_address;

  struct so_sockaddr_in address;
};

// A SETSOCKOPT input
struct so_setsockopt_input {
  uint32_t socket;
  uint32_t level;
  uint32_t option;
  uint32_t value_len;
  uint8_t value[];
};

// A GETSOCKOPT output, which holds the parameters and takes the value back
struct so_getsockopt_output {
  uint32_t socket;
  uint32_t level;
  uint32_t option;

  // 8 on the way in, the value's length on the way out
  uint32_t value_len;

  uint8_t value[8];
};

// The arguments vector of a SENDTO
struct so_sendto_arguments {
  uint32_t socket;
  uint32_t flags;

  // 1 when address applies
  uint32_t has_address;

  struct so_sockaddr_in address;
};

// A GETNAMEINFO input
struct so_getnameinfo_input {
  // Bits from enum so_nameinfo_flag
  uint32_t flags;

  // Always 1
  uint32_t has_address;

  struct so_sockaddr_in address;
};

// One POLL entry, 12 bytes
struct so_poll_entry {
  uint32_t socket;

  // Bits from enum so_poll_event
  uint32_t events;

  // The events that happened, written for every entry
  uint32_t returned_events;
};

// An ICMP_PING input
struct so_icmp_ping_input {
  uint32_t slot;

  // Always 1
  uint32_t has_address;

  uint64_t timeout_milliseconds;
  struct so_sockaddr_in address;
};

// KD's startup configuration, the STARTUP_CONFIG input, 0x70 bytes
struct so_startup_config {
  // Always SO_STARTUP_VERSION
  uint32_t version;

  // The client's allocator, called as alloc(name, size), which only the client uses
  uint32_t alloc;

  // The client's matching free(name, pointer, size), which only the client uses
  uint32_t free;

  // Bits from enum so_startup_flag
  uint32_t flags;

  // The static address, netmask and gateway, when DHCP is off
  uint32_t address;
  uint32_t netmask;
  uint32_t gateway;

  // DNS servers, which win over DHCP's while either is set
  uint32_t dns_servers[2];

  // Size of the TCP TIME_WAIT table, 0x34 bytes a connection, rounded up to 0x2000, 0x4000 or 0x8000
  uint32_t time_wait_buffer_size;

  // Size of the fragment reassembly buffer, which SO ignores for a fixed 0x10000
  uint32_t reassembly_buffer_size;

  // MTU, clamped to 68 to 1500, and 0 for 1500
  uint32_t mtu;

  // SO keeps it as 0 or at least 28 but never reads it back, and KD sends 0
  uint32_t unused0;

  // Pads the time to an 8 byte boundary
  uint32_t padding;

  // A new TCP socket's SO_TCP_R2 in seconds, 100 when 0 or less
  int64_t tcp_r2_seconds;

  // PPPoE peer id, password and service name, pointers to strings
  uint32_t pppoe_peer_id;
  uint32_t pppoe_password;
  uint32_t pppoe_service_name;

  // DHCP host name, a pointer to a string of up to 0x100 bytes
  uint32_t dhcp_host_name;

  // DHCP retries, 8 when 0 or less
  uint32_t dhcp_retries;

  // SO never reads them and KD leaves them 0
  uint32_t unused1[2];

  // PPPoE over Ethernet values SO stores in its LCP state, the second only when 1 to 3, and never reads back
  uint32_t pppoe_values[2];

  // SO never reads it and KD leaves it 0
  uint32_t unused2;

  // When nonzero, a PPPoE start with a peer id or password fails
  uint8_t pppoe_byte;

  // SO never reads them and KD leaves them 0
  uint8_t unused3[7];
};

// One route a SO_INTERFACE_OPTION_ROUTES read returns, 24 bytes
struct so_route {
  uint32_t destination;
  uint32_t netmask;
  uint32_t gateway;

  // Bits from enum so_route_flag
  uint32_t flags;

  // When a lookup last used the route, in kernel ticks
  uint64_t last_used;
};
