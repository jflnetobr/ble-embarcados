#include <services/ble_service.h>

uint8_t service_notification(struct bt_conn *conn, struct bt_gatt_subscribe_params *params, const void *buf, uint16_t len) {
  if (!buf) {
    printk("Ocorreu um erro ao receber os dados do periférico.\n");
    params->value_handle = 0U;
    return BT_GATT_ITER_CONTINUE;
  }

  char text_upper_case[len];
  memcpy(text_upper_case, buf, len);

  printk("Texto em caixa alta (recebido do periférico): %s\n", text_upper_case);

  return BT_GATT_ITER_CONTINUE;
}