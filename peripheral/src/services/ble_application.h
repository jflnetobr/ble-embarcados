#ifndef __BLE_APPLICATION_H__
#define __BLE_APPLICATION_H__

#include <bluetooth/bluetooth.h>
#include <bluetooth/gatt.h>
#include <kernel.h>

void ble_connected(struct bt_conn *conn, uint8_t err);
void ble_disconnected(struct bt_conn *conn, uint8_t reason);
int ble_init(void);

#endif // __BLE_APPLICATION_H__