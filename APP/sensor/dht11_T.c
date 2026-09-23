// dht11.c
#include "dht11_T.h"
#include "systick.h"

#define DHT11_PORT   GPIOA
#define DHT11_PIN    GPIO_PIN_1

static void DHT11_SetOutput(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

static void DHT11_SetInput(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;   // 外部通常也會有4.7k~10k上拉電阻
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

uint8_t DHT11_Init(void) {
    DWT_Delay_Init();
    DHT11_SetInput();
    return 1;
}

// 送出起始訊號，並確認DHT11有回應
static uint8_t DHT11_Start(void) {
    DHT11_SetOutput();
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
    HAL_Delay(20);  // 拉低至少18ms
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
    delay_us(30);
    DHT11_SetInput();

    // 等待DHT11拉低(response)，逾時保護避免卡死
    uint32_t timeout = 1000;
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET) {
        delay_us(1);
        if (--timeout == 0) return 0;
    }
    timeout = 1000;
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET) {
        delay_us(1);
        if (--timeout == 0) return 0;
    }
    timeout = 1000;
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET) {
        delay_us(1);
        if (--timeout == 0) return 0;
    }
    return 1; // 回應訊號正常
}

static uint8_t DHT11_ReadByte(void) {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        uint32_t timeout = 1000;
        while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET) {
            delay_us(1);
            if (--timeout == 0) return byte;
        }
        delay_us(40); // 40us後判斷高電位還在不在，決定是0還是1
        byte <<= 1;
        if (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET) {
            byte |= 1;
            timeout = 1000;
            while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET) {
                delay_us(1);
                if (--timeout == 0) break;
            }
        }
    }
    return byte;
}

uint8_t DHT11_Read(DHT11_Data *data) {
    if (!DHT11_Start()) return 0; // 沒收到回應，可能斷線或時序問題

    data->humidity_int = DHT11_ReadByte();
    data->humidity_dec = DHT11_ReadByte();
    data->temp_int      = DHT11_ReadByte();
    data->temp_dec       = DHT11_ReadByte();
    data->checksum       = DHT11_ReadByte();

    DHT11_SetInput();

    uint8_t sum = data->humidity_int + data->humidity_dec
                + data->temp_int + data->temp_dec;
    if (sum != data->checksum) return 0; // 校验失败

    return 1;
}