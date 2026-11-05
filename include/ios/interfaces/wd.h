#pragma once

#include <stdint.h>

// Device NCD and the System Menu open, one session at a time, with the state in the open mode
#define WD_DEVICE "/dev/net/wd/command"

// Open mode bits that hold the state, which must be WD_STATE_MAX or less
#define WD_OPEN_MODE_STATE_MASK 0xffff

// Highest state a client may open the device in
#define WD_STATE_MAX 6

// State NCD opens the device in, the only one that sends and receives frames
#define WD_STATE_STATION 3

// Open mode bit that selects raw buffers over iobufs, set by the PowerPC side
#define WD_OPEN_MODE_RAW (1u << 16)

// Disconnect reason for a link the client took down
#define WD_REASON_REQUESTED 0xf002

// Requests, all ioctlv. Buffers are big endian, and in iobuf mode a frame or scan buffer is an iobuf pointer
enum wd_request {
  WD_REQUEST_GET_STATE            = 0x1001, // Returns the state the client opened the device in
  WD_REQUEST_SET_LINK_STATE       = 0x1002, // Joins the configured network on 1 and leaves on 0, in states 1 to 6
  WD_REQUEST_GET_STATUS           = 0x1003, // Returns 1 while the link is up
  WD_REQUEST_SET_CONFIG           = 0x1004, // Stores the struct wd_config fields a struct wd_config_masks selects
  WD_REQUEST_GET_CONFIG           = 0x1005, // Writes the struct wd_config fields a struct wd_config_masks selects
  WD_REQUEST_SEND_BEACON          = 0x1006, // Sends a beacon in DS master mode
  WD_REQUEST_DISASSOC             = 0x1007, // Drops the DS child with the given MAC address, in states 1, 5 and 6
  WD_REQUEST_MP_SEND_FRAME        = 0x1008, // Sends a frame in DS master mode
  WD_REQUEST_SEND_FRAME           = 0x1009, // Sends an Ethernet frame of 14 or more bytes, with the link up
  WD_REQUEST_SCAN                 = 0x100a, // Scans with a struct wd_scan_parameters, writes a count and the entries
  WD_REQUEST_MEASURE_CHANNEL      = 0x100b, // Measures a channel's load
  WD_REQUEST_CALL_WL              = 0x100c, // Passes a call through to WL
  WD_REQUEST_GET_WL_STATUS        = 0x100d, // Writes a struct wd_wl_status and returns the last WL result
  WD_REQUEST_GET_INFO             = 0x100e, // Writes a struct wd_info
  WD_REQUEST_SET_BEACON           = 0x100f, // Sets the beacon in DS master mode
  WD_REQUEST_CHANGE_VTSF          = 0x1010, // Passes a halfword to WL, refused in states 0, 2 and 3
  WD_REQUEST_RECEIVE_FRAME        = 0x8000, // Writes the next received frame, parked until one arrives
  WD_REQUEST_RECEIVE_NOTIFICATION = 0x8001, // Writes the next struct wd_notification, parked until one is queued
};

// Results
enum wd_result {
  WD_ECLOSED = (int32_t)0x80008000, // Released by the session closing
  WD_EINVAL  = (int32_t)0x80008001, // Bad parameter
  WD_ESTATE  = (int32_t)0x80008002, // Bad state
  WD_EDRIVER = (int32_t)0x80008003, // WL error
  WD_ESHORT  = (int32_t)0x80008004, // Output too short, which loses a notification
  WD_ENOMEM  = (int32_t)0x80008005, // No memory
  WD_EFAIL   = -4,                  // Unknown request or wrong descriptor
};

// Notification record types
enum wd_notify {
  WD_NOTIFY_CONNECTED    = 0,
  WD_NOTIFY_DISCONNECTED = 1,
  WD_NOTIFY_IGNORED      = 4, // A 0x94 byte record NCD ignores, sent before and while connected
};

// Capability bits of a scan entry, of which NCD needs ESS set and IBSS clear
enum wd_capability {
  WD_CAPABILITY_ESS     = (1 << 0),
  WD_CAPABILITY_IBSS    = (1 << 1),
  WD_CAPABILITY_PRIVACY = (1 << 4),
};

// Privacy modes in wd_config
enum wd_privacy_mode {
  WD_PRIVACY_NONE     = 0,
  WD_PRIVACY_WEP40    = 1,
  WD_PRIVACY_WEP104   = 2,
  WD_PRIVACY_WPA_TKIP = 4,
  WD_PRIVACY_WPA2_AES = 5,
  WD_PRIVACY_WPA_AES  = 6,
};

// Bits of wd_config_masks.ess_sta, each selecting wd_config fields
enum wd_ess_sta_mask {
  WD_MASK_RESERVED0           = (1 << 0),
  WD_MASK_USE_ANTENNA         = (1 << 1),
  WD_MASK_SHORT_RETRY_LIMIT   = (1 << 2),
  WD_MASK_LONG_RETRY_LIMIT    = (1 << 3),
  WD_MASK_LIFETIME            = (1 << 4),
  WD_MASK_RTS_THRESHOLD       = (1 << 5),
  WD_MASK_FRAG_THRESHOLD      = (1 << 6),
  WD_MASK_SUPPORTED_RATES     = (1 << 7),
  WD_MASK_BASIC_RATES         = (1 << 8),
  WD_MASK_SSID                = (1 << 16),
  WD_MASK_SSID_MASK           = (1 << 17),
  WD_MASK_SSID_LEN            = (1 << 18),
  WD_MASK_MAX_CHANNEL_TIME    = (1 << 19),
  WD_MASK_PRIVACY             = (1 << 20),
  WD_MASK_BEACON_LOST_SECONDS = (1 << 21),
  WD_MASK_BSSID               = (1 << 22),
  WD_MASK_MAC_ADDRESS         = (1 << 23),
};

// Bits of wd_config_masks.mp_parent, each selecting a wd_mp_parent field
enum wd_mp_parent_mask {
  WD_MP_MASK_CONNECTION_TIMEOUT = (1 << 0),
  WD_MP_MASK_BEACON_PERIOD      = (1 << 1),
  WD_MP_MASK_MAX_NODES          = (1 << 2),
  WD_MP_MASK_AUTH_ALGORITHM     = (1 << 3),
  WD_MP_MASK_PRIVACY            = (1 << 4),
  WD_MP_MASK_CHANNEL            = (1 << 5),
  WD_MP_MASK_UNKNOWN0           = (1 << 6),
  WD_MP_MASK_UNKNOWN3           = (1 << 16),
  WD_MP_MASK_UNKNOWN2           = (1 << 17),
};

// A privacy block, 0x48 bytes
struct wd_privacy {
  // An enum wd_privacy_mode value
  uint16_t mode;

  // Read by neither WD's checks nor its join
  uint16_t reserved;

  // The key length 8 to 64 for WPA, or the key id 0 to 3 for WEP
  uint16_t key_len;

  // A WPA passphrase or its 64 hex digits, or four WEP keys packed at the key length
  uint8_t key[0x42];
};

// DS master mode settings, 0xd4 bytes, which NCD never sets
struct wd_mp_parent {
  // Nonzero
  uint16_t connection_timeout;

  // 10 to 1000
  uint16_t beacon_period;

  // 15 or less
  uint8_t max_nodes;

  // 0 or 1
  uint8_t auth_algorithm;

  // Unknown, WD_MP_MASK_UNKNOWN0
  uint16_t unknown0;

  // A channel in wd_info.channels
  uint8_t channel;

  // Unknown, no mask bit
  uint8_t unknown1[15];

  // Unknown, WD_MP_MASK_UNKNOWN2 and WD_MP_MASK_UNKNOWN3
  uint16_t unknown2;
  uint16_t unknown3;

  // Unknown, no mask bit
  uint8_t unknown4[0x70];

  // WEP only
  struct wd_privacy privacy;
};

// The configuration SetConfig and GetConfig carry, 0x180 bytes
struct wd_config {
  uint16_t reserved0;

  // 0 or 1
  uint16_t use_antenna;

  // Each below 0x100
  uint16_t short_retry_limit;
  uint16_t long_retry_limit;

  // How long WL keeps trying to send a packet, in milliseconds, NCD writes 10000
  uint16_t lifetime;

  // Each nonzero, NCD writes 2345 and 2346
  uint16_t rts_threshold;
  uint16_t frag_threshold;

  // Rate set NCD writes
  uint16_t supported_rates;

  // Nonzero in its low 12 bits, NCD writes 3
  uint16_t basic_rates;

  // No mask bit selects it and NCD leaves it 0
  uint16_t reserved2;

  struct wd_privacy privacy;

  uint8_t ssid[32];
  uint8_t ssid_mask[32];

  // 32 or less
  uint8_t ssid_len;

  // Seconds without beacons before the link counts as lost, NCD writes 10
  uint8_t beacon_lost_seconds;

  // Longest time on a channel in a scan, 10 to 1000 milliseconds, NCD writes 40
  uint16_t max_channel_time;

  // The access point NCD selected from the scan
  uint8_t bssid[6];

  // The station's own MAC address
  uint8_t mac_address[6];

  struct wd_mp_parent mp_parent;
};

// The masks SetConfig and GetConfig take beside a struct wd_config
struct wd_config_masks {
  // Bits from enum wd_mp_parent_mask
  uint32_t mp_parent;

  // Bits from enum wd_ess_sta_mask, 0x00ff01fc from NCD to connect and 0x00800000 to set the MAC address alone
  uint32_t ess_sta;
};

// The block GetInfo returns, 0x90 bytes
struct wd_info {
  uint8_t mac_address[6];

  // Channels the station may use, bit n for channel n
  uint16_t channels;

  // Channels allowed in DS mode, bit n for channel n
  uint16_t ds_channels;

  char country[4];
  uint8_t current_channel;
  uint8_t initialized;
  char driver_version[0x80];
};

// The parameters a Scan takes, 0x4e bytes
struct wd_scan_parameters {
  // Channels to scan, bit n for channel n
  uint16_t channels;

  // Longest time on a channel, 10 to 1000 milliseconds
  uint16_t max_channel_time;

  // The access point to look for, or all ones for any
  uint8_t bssid[6];

  // 0 for an active scan, 1 for a passive one
  uint16_t scan_type;

  // 32 or less
  uint16_t ssid_len;

  uint8_t ssid[32];
  uint8_t ssid_mask[32];
};

// One network in a scan result, 0x40 bytes and then its extra elements
struct wd_scan_entry {
  // The entry's length in halfwords, extra elements included
  uint16_t len_halfwords;

  // Signal strength
  uint16_t strength;

  uint8_t bssid[6];
  uint16_t ssid_len;
  uint8_t ssid[32];

  // Bits from enum wd_capability
  uint16_t capability;

  uint16_t basic_rates;
  uint16_t supported_rates;
  uint16_t beacon_period;
  uint16_t dtim_period;
  uint16_t channel;
  uint16_t cfp_period;
  uint16_t cfp_max_duration;
  uint16_t extra_len;
  uint16_t extra_count;
};

// A notification record, 0x34 bytes connected and 0x14, up to the SSID, disconnected
struct wd_notification {
  // An enum wd_notify value
  uint32_t type;

  // The record's length in bytes
  uint32_t len;

  uint8_t bssid[6];
  uint16_t association_id;

  // The reason for a disconnect, or the status of a connect
  uint16_t reason;

  uint16_t padding;
  uint8_t ssid[32];
};

// The WL failure record GetWLStatus writes, 0x10 bytes
struct wd_wl_status {
  // 1 when a WL command has failed since the last GetWLStatus, which clears it
  uint32_t failed;

  // WL commands that have failed
  uint32_t failure_count;

  // The last failure's WL error code and result
  uint16_t error_code;
  uint16_t result;

  uint16_t reserved[2];
};
