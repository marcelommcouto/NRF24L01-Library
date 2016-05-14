#ifndef _NRF24HAL_H_
#define _NRF24HAL_H_

/** ************************************************************************
 * Modulo: nrf24hal 
 * @file nrf24hal.h
 * @headerfile nrf24hal.h
 * @author Marcelo Martins Maia do Couto - Email: marcelo.m.maia@gmail.com
 * @date Apr 4, 2015
 *
 * @brief Substitua este texto por uma descrição breve deste módulo.
 *
 * Substitua este texto pela descrição completa deste módulo.
 * Este módulo é um modelo para a criação de novos módulos. Ele contém
 * toda a estrutura que um módulo deve conter, sendo composto pelos arquivos:
 *   - nrf24hal.c;
 *   - nrf24hal.h.
 *
 * @copyright Copyright 2015 M3C Tecnologia
 * @copyright Todos os direitos reservados.
 *
 * @note
 *  - Não sobrescreva os arquivos de template do módulo. Implemente um novo
 *    módulo sobre uma cópia do template.
 *  - Leia a documentação do @b Doxygen para maiores informações sobre o
 *    funcionamento dos recursos de documentação de código.
 *
 * @warning
 *  - É altamente recomendado manter todos os arquivos de template como
 *    somente-leitura, evitando assim que eles sejam sobrescritos ao serem
 *    utilizados.
 *
 * @attention
 *  - A descrição de cada módulo como um todo deve ser realizada no arquivo
 *    ".h" do mesmo.
 *  - Cada módulo de um projeto de software deve conter, pelo menos, um
 *    arquivo ".h" e um ".c".
 * @pre 
 *   Coloque algum pré-requisito para utilizar este módulo aqui.
 *
 ******************************************************************************/

/*
 * Inclusão de arquivos de cabeçalho da ferramenta de desenvolvimento.
 * Por exemplo: '#include <stdlib.h>'.
 */
#include <stdlib.h>
#include <stdbool.h>

/*
 * Inclusão de arquivos de cabeçalho sem um arquivo ".c" correspondente.
 * Por exemplo: '#include "stddefs.h"'.
 */
#include "LPC17xx.h"

/*
 * Inclusão de arquivos de cabeçalho de outros módulos utilizados por este.
 * Por exemplo: '#include "serial.h"'.
 */

/*******************************************************************************
 *                           DEFINICOES E MACROS							   *
 ******************************************************************************/
#define SSP_SR_TFE      ((uint32_t)(1<<0)) /** SSP status TX FIFO Empty bit */
#define SSP_SR_TNF      ((uint32_t)(1<<1)) /** SSP status TX FIFO not full bit */
#define SSP_SR_RNE      ((uint32_t)(1<<2)) /** SSP status RX FIFO not empty bit */
#define SSP_SR_RFF      ((uint32_t)(1<<3)) /** SSP status RX FIFO full bit */
#define SSP_SR_BSY      ((uint32_t)(1<<4)) /** SSP status SSP Busy bit */
#define SSP_SR_BITMASK	((uint32_t)(0x1F)) /** SSP SR bit mask */

/* Pin/Port Definition. */
#define SSPPORT			LPC_SSP0
#define SSPPORTSSELNUM	0
#define SSPSSELPIN		16
#define SSPPORTNUM		0
#define SSPCLKPIN		15
#define SSPMISOPIN		17
#define SSPMOSIPIN 		18

#define RF24CEPORTNUM	0
#define RF24CEPORTPIN	11
#define RF24IRQPORTNUM	0
#define RF24IRQPORTPIN	12

#define HIGH 			1
#define LOW 			0

#define NRF24_MAX_LENGTH_PAYLOAD 	32

/*******************************************************************************
 *                     ESTRUTURAS E DEFINICOES DE TIPOS						   *	
 ******************************************************************************/

/*******************************************************************************
 *                       VARIAVEIS PUBLICAS (Globais)						   *
 ******************************************************************************/

/*******************************************************************************
 *                      PROTOTIPOS DAS FUNCOES PUBLICAS						   *
 ******************************************************************************/

/**
 * Set chip enable
 *
 * @param level HIGH to actively begin transmission or LOW to put in standby.  Please see data sheet
 * for a much more detailed description of this pin.
 */
void ce(int level);

void SysTick_Handler(void);
void delayInit(void);
void delayUs(int us);
uint32_t delayGetTick(void);

bool nrf24HalInit(void);
void nrf24ReadRegisterBuf(uint8_t reg, uint8_t* buf, uint8_t len);
void nrf24ReadPayload(uint8_t* buf, uint8_t len);
void nrf24ReadRegister(uint8_t reg, uint8_t *value);

uint8_t nrf24WriteRegisterBuf(uint8_t reg, uint8_t* buf, uint8_t len);
uint8_t nrf24WriteRegister(uint8_t reg, uint8_t *value);
uint8_t nrf24WriteRegisterOnly(uint8_t reg);
bool nrf24WritePayload(uint8_t* buf, uint8_t len);

/*******************************************************************************
 *                                   EOF									   *	
 ******************************************************************************/
#endif
