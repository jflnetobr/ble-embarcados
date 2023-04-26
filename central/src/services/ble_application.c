#include <services/ble_application.h>
#include <services/ble_service.h>

uint16_t uart_write = 0;

struct bt_conn *main_conn = NULL;
struct bt_conn_cb conn_cb = {
  .connected = ble_connected,
  .disconnected = ble_disconnected,
};

struct bt_uuid_128 uuid = BT_UUID_INIT_128(0);
struct bt_gatt_discover_params discover_params;
struct bt_gatt_subscribe_params subscribe_params;

struct bt_uuid_128 primary_service_uuid = BT_UUID_INIT_128(BT_UUID_CUSTOM_SERVICE_VAL);
struct bt_uuid_128 notify_characteristic_uuid = BT_UUID_INIT_128(BT_UUID_CUSTOM_SERVICE_NOTIFY_VALL);
struct bt_uuid_128 write_characteristic_uuid = BT_UUID_INIT_128(BT_UUID_CUSTOM_SERVICE_WRITE_VALL);

void ble_connected(struct bt_conn *conn, uint8_t err) {
  char addr[BT_ADDR_LE_STR_LEN];

  bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

  if (err) {
    printk("Falha ao conectar. Endereço: %s. Código de erro: %u\n", addr, err);

    bt_conn_unref(main_conn);
    main_conn = NULL;

    ble_start_scan();
    return;
  }

  printk("Conectado ao dispositivo com endereço: %s\n", addr);

  if (conn == main_conn) {
    printk("Conectado com sucesso. Endereço: %s\n", addr);

    memcpy(&uuid, &primary_service_uuid.uuid, sizeof(uuid));
    discover_params.type = BT_GATT_DISCOVER_PRIMARY;
    discover_params.uuid = &uuid.uuid;
    discover_params.end_handle = 0xffff;
    discover_params.start_handle = 0x0001;
    discover_params.func = discover_func;

    err = bt_gatt_discover(main_conn, &discover_params);
    if (err) {
      printk("Falha ao tentar descobrir características. Código de erro: %u\n", err);
      return;
    }
  }
}

void ble_disconnected(struct bt_conn *conn, uint8_t reason)
{
  char addr[BT_ADDR_LE_STR_LEN];

  if (conn != main_conn) {
    return;
  }

  bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

  printk("Dispositivo com endereço %s desconectado. Motivo: 0x%02x\n", addr, reason);

  bt_conn_unref(main_conn);
  main_conn = NULL;

  ble_start_scan();
}

uint8_t discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr, struct bt_gatt_discover_params *params) {
  int err;

  if (!attr) {
    printk("Todas as características foram descobertas\n");
    (void) memset(params, 0, sizeof(*params));
    return BT_GATT_ITER_STOP;
  }

  if (!bt_uuid_cmp(discover_params.uuid, &primary_service_uuid.uuid)) {
    memcpy(&uuid, &notify_characteristic_uuid.uuid, sizeof(uuid));

    discover_params.uuid = &uuid.uuid;
    discover_params.start_handle = attr->handle + 1;
    discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;

    err = bt_gatt_discover(conn, &discover_params);
    if (err) {
      printk("Falha ao descobrir características. Erro: %d\n", err);
    }
  } else if (!bt_uuid_cmp(discover_params.uuid, &notify_characteristic_uuid.uuid)) {
    memcpy(&uuid, &write_characteristic_uuid.uuid, sizeof(uuid));

    discover_params.uuid = &uuid.uuid;
    discover_params.start_handle = attr->handle + 1;
    discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;
    subscribe_params.value_handle = bt_gatt_attr_value_handle(attr);

    err = bt_gatt_discover(conn, &discover_params);
    if (err) {
      printk("Falha ao descobrir características. Erro: %d\n", err);
    }
  } else if (!bt_uuid_cmp(discover_params.uuid, &write_characteristic_uuid.uuid)) {
    memcpy(&uuid, BT_UUID_GATT_CCC, sizeof(uuid));

    discover_params.uuid = &uuid.uuid;
    discover_params.start_handle = attr->handle + 1;
    discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;
    uart_write = bt_gatt_attr_value_handle(attr);

    err = bt_gatt_discover(conn, &discover_params);
    if (err) {
      printk("Falha ao descobrir características. Erro: %d\n", err);
    }
  } else {
    subscribe_params.notify = service_notification;
    subscribe_params.value = BT_GATT_CCC_NOTIFY;
    subscribe_params.ccc_handle = attr->handle;

    err = bt_gatt_subscribe(conn, &subscribe_params);
    if (err && err != -EALREADY) {
      printk("Falha ao tentar inscrever. Erro: %d\n", err);
    } else {
      printk("Inscrito com sucesso\n");
    }

    return BT_GATT_ITER_STOP;
  }

  return BT_GATT_ITER_STOP;
}

void ble_device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type, struct net_buf_simple *buf) {
  int err;

  if (main_conn) {
		return;
	}

  if (type != BT_GAP_ADV_TYPE_ADV_IND && type != BT_GAP_ADV_TYPE_ADV_DIRECT_IND) {
    return;
  }

  if (rssi < -70) {
    return;
  }
  
  if (bt_le_scan_stop()) {
		return;
	}

	err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN, BT_LE_CONN_PARAM_DEFAULT, &main_conn);
	if (err) {
		printk("Não foi possível criar a conexão com o dispositivo. Erro: %d\n", err);

		ble_start_scan();
	}
}

void ble_start_scan(void) {
  int err;

  struct bt_le_scan_param scanParameters = {
    .type = BT_LE_SCAN_TYPE_ACTIVE,
    .options = BT_LE_SCAN_OPT_NONE,
    .interval = BT_GAP_SCAN_FAST_INTERVAL,
    .window = BT_GAP_SCAN_FAST_WINDOW,
  };

  err = bt_le_scan_start(&scanParameters, ble_device_found);
  if (err) {
    printk("Não foi possível iniciar o escaneamento. Erro: %d\n", err);
    return;
  }

  printk("Escaneamento iniciado\n");
}

void read_input(void) {
  k_sleep(K_MSEC(1000));

  console_getline_init();

  while (true) {
    printk("Digite a mensagem: ");
    char *string = console_getline();

    bt_gatt_write_without_response(main_conn, uart_write, string, strlen(string), false);

    k_sleep(K_MSEC(500));
  }
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

  ble_start_scan();
  read_input();

  return 0;
}