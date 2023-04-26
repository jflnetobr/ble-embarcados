
# Projeto de comunicação usando BLE no Zephyr

Projeto desenvolvido para a disciplina de Sistemas Embarcados - 2022.2, no Instituto de Computação da Universidade de Alagoas (IC/UFAL). Este README busca descrever o trabalho de forma mais técnica e voltada para o desenvolvimento.

#### Professor
- Erick de Andrande Barboza

#### Alunos 
- Bruno Lemos de Lima
- José Ferreira Leite Neto
- Lucas Lemos Cerqueira de Freitas
- Maria Fernanda Herculano Machado da Silva

## Como rodar o projeto

Para executar o projeto, siga os passos abaixo:

1. Compile e faça o build do firmware tanto para o dispositivo central quanto para o dispositivo periférico.

2. Abra o terminal, na pasta raiz do projeto, executando o seguinte comando no terminal: renode build.resc

3. Inicie a simulação executando o comando start no console do Renode.

4. No terminal do dispositivo central, digite o texto que deseja enviar para o dispositivo periférico.

## Funcionamento do projeto

  ### Dispositivo central
  O dispositivo central é responsável por iniciar a comunicação BLE, realizar o scan dos dispositivos periféricos, conectar-se a um deles e enviar uma mensagem.

  O fluxo de funcionamento do dispositivo central é o seguinte:

  1. Inicia o BLE (bt_enable).
  2. Registra um callback para eventos de conexão (bt_conn_cb_register).
  3. Inicia a varredura por dispositivos periféricos (bt_le_scan_start), observando e conectando-se a qualquer dispositivo que envie um anúncio. Quando um dispositivo periférico é encontrado, é acionada a função ble_device_found, que para a varredura de dispositivos.
  4. Lê a entrada do usuário do terminal.
  5. Envia a mensagem sem esperar uma resposta (bt_gatt_write_without_response).
  6. Aguarda o callback para indicar a conexão com o dispositivo periférico (bt_conn_le_create).
  7. Quando o dispositivo periférico está conectado, define uma função de notificação (ble_service.c) que converte o texto da mensagem para letras maiúsculas e a envia de volta ao dispositivo central (bt_gatt_notify).
  8. Quando o dispositivo periférico é desconectado, desconecta e imprime o motivo da desconexão, em seguida, recomeça a varredura por dispositivos periféricos.

  ### Dispositivo periférico

  O dispositivo periférico é responsável por iniciar o BLE, configurar o callback para a conexão e desconexão BLE e notificar o dispositivo central quando recebe uma mensagem.

  O fluxo de funcionamento do dispositivo periférico é o seguinte:

  1. Inicia o BLE (bt_enable)
  2. Configura os callbacks para eventos de conexão (conectado) e desconexão (desconectado).
  3. Quando o BLE estiver pronto, inicia a publicidade para informar suas funções.
  4. Quando um dispositivo central se conecta ao dispositivo periférico, incrementa o contador de referência de conexão (bt_conn_ref).
  5. Quando um dispositivo central se desconecta do dispositivo periférico, decrementa o contador de referência de conexão (bt_conn_ref).
  6. Quando há mudança de configuração no Client Characteristic Configuration (CCC), o dispositivo periférico se conecta ao servidor (bt_ccc_cfg_changed).
  7. Quando uma mensagem é recebida, o dispositivo periférico converte o texto para letras maiúsculas e notifica o dispositivo central (bt_gatt_notify).



