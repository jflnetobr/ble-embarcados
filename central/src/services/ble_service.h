#ifndef __BLE_SERVICE_H__
#define __BLE_SERVICE_H__

#include <bluetooth/gatt.h>

uint8_t service_notification(struct bt_conn *conn, struct bt_gatt_subscribe_params *params, const void *buf, uint16_t len);

#endif // __BLE_SERVICE_H__