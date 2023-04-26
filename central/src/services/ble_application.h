#ifndef __BLE_APPLICATION_H__
#define __BLE_APPLICATION_H__

#include <bluetooth/gatt.h>
#include <sys/byteorder.h>
#include <console/console.h>

#define BT_UUID_CUSTOM_SERVICE_VAL BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)
#define BT_UUID_CUSTOM_SERVICE_NOTIFY_VALL BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef1)
#define BT_UUID_CUSTOM_SERVICE_WRITE_VALL BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef2)

void ble_connected(struct bt_conn *conn, uint8_t err);
void ble_disconnected(struct bt_conn *conn, uint8_t reason);
uint8_t discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr, struct bt_gatt_discover_params *params);
void ble_device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type, struct net_buf_simple *buf);
void ble_start_scan(void);
void read_input(void);
int ble_init(void);

#endif // __BLE_APPLICATION_H__