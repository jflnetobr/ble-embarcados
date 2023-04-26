#ifndef __BLE_SERVICE_H__
#define __BLE_SERVICE_H__

#include <bluetooth/gatt.h>

#define BT_UUID_CUSTOM_SERVICE_VAL BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)
#define BT_UUID_CUSTOM_SERVICE_NOTIFY_VALL BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef1)
#define BT_UUID_CUSTOM_SERVICE_WRITE_VALL BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef2)

void bt_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value);
int write_bt(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags);

#endif // __BLE_SERVICE_H__