# Introdução
Esse projeto IoT implementar uma solução de integração de oxímetros MAX30100 com Wifi via uma ESP32 e protocolo MQTT

Todo o backend do Broker é desenvolvido em ASP.NET e também existe um Dashboard para visualização desses dados com WPF, utilizando .NET 8.0

## Lista de componentes utilizados
- ESP32 WROOM DEVKIT
- MAX30100 (Podendo ser MAX30102)
- Buzzer

## Esquema da montagem
![alt text for screen readers](/images/montagem.png "Dashboard")\
O resultado deve ser algo como:
![alt text for screen readers](/images/montagem-fisica.png "Dashboard")\
É possível ver na imagem um jumper no sensor MAX30100, isso se deve ao mau funcionamento do sensor no alto nível lógico da ESP, para contornar o problema foi necessário montar o seguinte jumper:
![alt text for screen readers](/images/jumper.png "Dashboard")


## Funcionamento
Um computador deve funcionar de servidor e dashboard, rodando as aplicações .NET enquanto a ESP32 será cliente deste servidor, capturará os dados coletados pelo MAX30100, os enviará para o Broker, que por sua vez sala esses dados que posteriormente serão consumidos pelo dashboard: <br>
![alt text for screen readers](/images/dashboard.png "Dashboard")

## Configuração
Para usar, o usuário precisa que a ESP32 e o Server do Broker estejam na mesma rede Wi-Fi, atualizando os dados do .ino com a rede nova.
Setup no Ino
```
const char* ssid = "SEU_SSID";
const char* password = "SUA_SENHA";
const char* mqtt_server = "IP_SERVIDOR_BROKER";
```

Além disso, para o funcionamento do script da ESP serão necessárias as seguintes blbiotecas:
- MAX30100lib by OXullo
- PubSubClient by Nick O'Leary

Bom proveito!
