#include <services/ble_service.h>

struct bt_uuid_128 primary_service_uuid = BT_UUID_INIT_128(BT_UUID_CUSTOM_SERVICE_VAL);
struct bt_uuid_128 notify_characteristic_uuid = BT_UUID_INIT_128(BT_UUID_CUSTOM_SERVICE_NOTIFY_VALL);
struct bt_uuid_128 write_characteristic_uuid = BT_UUID_INIT_128(BT_UUID_CUSTOM_SERVICE_WRITE_VALL);

BT_GATT_SERVICE_DEFINE(primary_service,
  BT_GATT_PRIMARY_SERVICE(&primary_service_uuid.uuid),
  BT_GATT_CHARACTERISTIC(&notify_characteristic_uuid.uuid,
                         BT_GATT_CHRC_NOTIFY,
                         BT_GATT_PERM_NONE,
                         NULL,
                         NULL,
                         NULL),
  BT_GATT_CHARACTERISTIC(&write_characteristic_uuid.uuid,
                         BT_GATT_CHRC_WRITE,
                         BT_GATT_PERM_WRITE,
                         NULL,
                         write_bt,
                         NULL),
  BT_GATT_CCC(bt_ccc_cfg_changed,
              BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

void bt_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value) {
	ARG_UNUSED(attr);

	bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);
  printk("Notificações %s\n", notif_enabled ? "habilitadas" : "desabilitadas");
}

int write_bt(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags) {
  ARG_UNUSED(attr);

  int err;

  char text[len];
  memcpy(text, buf, len);

  char text_upper_case[len];
  for (int i = 0; i < len; i++) {
    if (text[i] >= 97 && text[i] <= 122) {
      text_upper_case[i] = text[i] - 32;
    } else {
      text_upper_case[i] = text[i];
    }
  }

  err = bt_gatt_notify(conn, &primary_service.attrs[1], text_upper_case, len);
  if (err) {
    printk("Ocorreu um erro ao tentar notificar o cliente. Erro: %d\n", err);
    return err;
  }

  return 0;
}