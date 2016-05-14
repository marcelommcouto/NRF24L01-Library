/** 
 *   Modulo: nrf24hal
 *   @file nrf24hal.c
 *   Veja nrf24hal.h para mais informações.
 ******************************************************************************/

/*******************************************************************************
 *                             MODULOS UTILIZADOS							   *
 ******************************************************************************/

/*
 * Inclusão de arquivos de cabeçalho da ferramenta de desenvolvimento.
 * Por exemplo: '#include <stdlib.h>'.
 */
#include <stdint.h>   /* Para as definições de uint8_t/uint16_t */
#include <stdbool.h>  /* Para as definições de true/false */ 

/*
 * Inclusão de arquivos de cabeçalho sem um arquivo ".c" correspondente.
 * Por exemplo: 
 * #include "stddefs.h" 
 * #include "template_header.h"
 */
#include "nrf24l01.h"

/*
 * Inclusão de arquivos de cabeçalho de outros módulos utilizados por este.
 * Por exemplo: '#include "serial.h"'.
 */
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_ssp.h"

/*
 * Inclusão dos arquivos ".tab.h" deste módulo.
 * Por exemplo: 
 * #include "barcode.tab.h"
 * #include "template.tab.h"
 */

/*
 * Inclusão do arquivo de cabeçalho deste módulo.
 */
#include "nrf24hal.h"

/*******************************************************************************
 *                     CONSTANTES E DEFINICOES DE MACRO						   *
 ******************************************************************************/

/*******************************************************************************
 *                      ESTRUTURAS E DEFINIÇÕES DE TIPOS					   *
 ******************************************************************************/

/*******************************************************************************
 *                        VARIÁVEIS PUBLICAS (Globais)						   *
 ******************************************************************************/
uint32_t usticks;

/*******************************************************************************
 *                  DECLARACOES DE VARIAVEIS PRIVADAS (static)				   *
 ******************************************************************************/

/*******************************************************************************
 *                   PROTOTIPOS DAS FUNCOES PRIVADAS (static)				   *
 ******************************************************************************/

/*******************************************************************************
 *                      IMPLEMENTACAO DAS FUNCOES PUBLICAS					   *
 ******************************************************************************/
bool nrf24HalInit(void)
{
	SSP_CFG_Type SSP_ConfigStruct;
	PINSEL_CFG_Type pincfg;

	/* Inicializando o Timer para as medidas de tempo da comunicacao do rf. */
	delayInit();

	/* Initialize Pins for CE. */
	pincfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	pincfg.Pinmode = PINSEL_PINMODE_NORMAL;
	pincfg.Portnum = RF24CEPORTNUM;
	pincfg.Pinnum = RF24CEPORTPIN;
	PINSEL_ConfigPin(&pincfg);
	GPIO_SetDir(RF24CEPORTNUM, (1 << RF24CEPORTPIN), 1);
	GPIO_ClearValue(RF24CEPORTNUM, (1 << RF24CEPORTPIN));

	/* Initialize SPI port 0. */
	pincfg.Funcnum = PINSEL_FUNC_2;
	pincfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	pincfg.Pinmode = PINSEL_PINMODE_PULLUP;
	pincfg.Portnum = SSPPORTNUM;
	pincfg.Pinnum = SSPCLKPIN;
	PINSEL_ConfigPin(&pincfg);
	pincfg.Pinnum = SSPMISOPIN;
	PINSEL_ConfigPin(&pincfg);
	pincfg.Pinnum = SSPMOSIPIN;
	PINSEL_ConfigPin(&pincfg);
	pincfg.Funcnum = PINSEL_FUNC_0;
	pincfg.Portnum = SSPPORTSSELNUM;
	pincfg.Pinnum = SSPSSELPIN;
	PINSEL_ConfigPin(&pincfg);

	SSP_ConfigStructInit(&SSP_ConfigStruct);

	GPIO_SetDir(SSPPORTSSELNUM, (1 << SSPSSELPIN), 1);
	GPIO_SetValue(SSPPORTSSELNUM, (1 << SSPSSELPIN));

	/* Setting SPI velocity in 8MHz. */
	SSP_ConfigStruct.ClockRate = 8000000;

	/* Inicializa o periférico SSP com os parâmetros da estrutura SSP_ConfigStruct. */
	SSP_Init(SSPPORT, &SSP_ConfigStruct);

	/* Habilita o periférico SSP. */
	SSP_Cmd(SSPPORT, ENABLE);

	return (true);
}

/****************************************************************************/
void nrf24ReadRegister(uint8_t reg, uint8_t *value)
{
	SSP_DATA_SETUP_Type xferConfig;

	reg = (R_REGISTER | (REGISTER_MASK & reg));

	xferConfig.tx_data = &reg;
	xferConfig.rx_data = NULL;
	xferConfig.length  = 2;

	GPIO_ClearValue(SSPPORTSSELNUM, (1 << SSPSSELPIN));
	SSP_ReadWrite(SSPPORT, &xferConfig, SSP_TRANSFER_POLLING);
	GPIO_SetValue(SSPPORTSSELNUM, (1 << SSPSSELPIN));
}

/****************************************************************************/
void nrf24ReadRegisterBuf(uint8_t reg, uint8_t *buf, uint8_t len)
{
	SSP_DATA_SETUP_Type xferConfig;

	reg = (R_REGISTER | (REGISTER_MASK & reg));

	xferConfig.tx_data = &reg;
	xferConfig.rx_data = buf;
	xferConfig.length  = (len + 1);

	GPIO_ClearValue(SSPPORTSSELNUM, (1 << SSPSSELPIN));
	SSP_ReadWrite(SSPPORT, &xferConfig, SSP_TRANSFER_POLLING);
	GPIO_SetValue(SSPPORTSSELNUM, (1 << SSPSSELPIN));
}

/****************************************************************************/
void nrf24ReadPayload(uint8_t *buf, uint8_t len)
{
	uint8_t plcmd = R_RX_PAYLOAD;
	SSP_DATA_SETUP_Type xferConfig;

	xferConfig.tx_data = &plcmd;
	xferConfig.rx_data = buf;
	xferConfig.length  = (len + 1);

	GPIO_ClearValue(SSPPORTSSELNUM, (1 << SSPSSELPIN));
	SSP_ReadWrite(SSPPORT, &xferConfig, SSP_TRANSFER_POLLING);
	GPIO_SetValue(SSPPORTSSELNUM, (1 << SSPSSELPIN));
}

/****************************************************************************/
uint8_t nrf24WriteRegisterOnly(uint8_t reg)
{
	SSP_DATA_SETUP_Type xferConfig;
	uint8_t result = 0;

	xferConfig.tx_data = &reg;
	xferConfig.rx_data = &result;
	xferConfig.length  = 1;

	GPIO_ClearValue(SSPPORTSSELNUM, (1 << SSPSSELPIN));
	SSP_ReadWrite(SSPPORT, &xferConfig, SSP_TRANSFER_POLLING);
	GPIO_SetValue(SSPPORTSSELNUM, (1 << SSPSSELPIN));

	return(result);
}

/****************************************************************************/
uint8_t nrf24WriteRegister(uint8_t reg, uint8_t *value)
{
	SSP_DATA_SETUP_Type xferConfig;
	uint8_t result = 0;

	reg = (W_REGISTER | (REGISTER_MASK & reg));

	GPIO_ClearValue(SSPPORTSSELNUM, (1 << SSPSSELPIN));

	xferConfig.tx_data = &reg;
	xferConfig.rx_data = NULL;
	xferConfig.length  = 1;
	SSP_ReadWrite(SSPPORT, &xferConfig, SSP_TRANSFER_POLLING);

	xferConfig.tx_data = value;
	xferConfig.rx_data = NULL;
	xferConfig.length  = 1;
	SSP_ReadWrite(SSPPORT, &xferConfig, SSP_TRANSFER_POLLING);

	GPIO_SetValue(SSPPORTSSELNUM, (1 << SSPSSELPIN));

	return(result);
}

/****************************************************************************/
uint8_t nrf24WriteRegisterBuf(uint8_t reg, uint8_t *buf, uint8_t len)
{
	SSP_DATA_SETUP_Type xferConfig;
	uint8_t result = 0;

	reg = (W_REGISTER | (REGISTER_MASK & reg));

	GPIO_ClearValue(SSPPORTSSELNUM, (1 << SSPSSELPIN));

	xferConfig.tx_data = &reg;
	xferConfig.rx_data = NULL;
	xferConfig.length  = 1;
	SSP_ReadWrite(SSPPORT, &xferConfig, SSP_TRANSFER_POLLING);

	xferConfig.tx_data = buf;
	xferConfig.rx_data = NULL;
	xferConfig.length  = len;
	SSP_ReadWrite(SSPPORT, &xferConfig, SSP_TRANSFER_POLLING);

	GPIO_SetValue(SSPPORTSSELNUM, (1 << SSPSSELPIN));

	return(result);
}

/****************************************************************************/
bool nrf24WritePayload(uint8_t *buf, uint8_t len)
{
	uint8_t result = W_TX_PAYLOAD;
	SSP_DATA_SETUP_Type xferConfig;

	/* Check. */
	if(len > NRF24_MAX_LENGTH_PAYLOAD) return(false);

	GPIO_ClearValue(SSPPORTSSELNUM, (1 << SSPSSELPIN));

	xferConfig.tx_data = &result;
	xferConfig.rx_data = NULL;
	xferConfig.length  = 1;
	SSP_ReadWrite(SSPPORT, &xferConfig, SSP_TRANSFER_POLLING);

	xferConfig.tx_data = buf;
	xferConfig.rx_data = NULL;
	xferConfig.length  = len;
	SSP_ReadWrite(SSPPORT, &xferConfig, SSP_TRANSFER_POLLING);

	GPIO_SetValue(SSPPORTSSELNUM, (1 << SSPSSELPIN));

	return (true);
}

/****************************************************************************/
void ce(int level)
{
	if(level)
	{
		GPIO_SetValue(RF24CEPORTNUM, (1 << RF24CEPORTPIN));
	}
	else
	{
		GPIO_ClearValue(RF24CEPORTNUM, (1 << RF24CEPORTPIN));
	}
}

/****************************************************************************/
void SysTick_Handler(void)
{
	usticks++;
}

/****************************************************************************/
void delayInit(void)
{
	SysTick_Config(SystemCoreClock / 1000000);
}

/****************************************************************************/
void delayUs(int us)
{
	usticks = 0;
	while (usticks < us);
}

/****************************************************************************/
uint32_t delayGetTick(void)
{
	return(usticks);
}

/******************************************************************************
 *                    IMPLEMENTACAO DAS FUNCOES PRIVADAS					  *
 *****************************************************************************/

/******************************************************************************	
 *                                    EOF                                     *
 *****************************************************************************/
