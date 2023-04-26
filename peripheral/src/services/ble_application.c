#include <services/ble_application.h>
#include <services/ble_service.h>

const struct bt_data ad[] = {
  BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
  BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_CUSTOM_SERVICE_VAL),
};

struct bt_conn *default_conn = NULL;
struct bt_conn_cb conn_cb = {
  .connected = ble_connected,
  .disconnected = ble_disconnected,
};

void ble_connected(struct bt_conn *conn, uint8_t err) {
  if (err) {
    printk("Falha ao conectar ao dispositivo (err 0x%02x)\n", err);
  } else {
    if (default_conn) {
      printk("Já existe uma conexão\n");
      return;
    }

    default_conn = bt_conn_ref(conn);
    printk("Conectado\n");
  }
}

void ble_disconnected(struct bt_conn *conn, uint8_t reason) {
	printk("Desconectado (reason %u)\n", reason);

  bt_conn_unref(default_conn);
  default_conn = NULL;
}

void ble_ready(void) {
  int err;

  err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
  if (err) {
    printk("Falha ao tentar inicializar a divulgação. Erro: %d\n", err);
    return;
  }

  printk("Divulgação inicializada com sucesso\n");
}

int ble_init(void) {
  int err;

  err = bt_enable(NULL);
  if (err) {
    printk("Falha ao tentar inicializar o Bluetooth. Erro: %d\n", err);
    return err;
  }

  printk("Bluetooth inicializado com sucesso\n");
  
  bt_conn_cb_register(&conn_cb);

  ble_ready();

  return 0;
}