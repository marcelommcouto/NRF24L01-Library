#ifndef _NRF24LIB_H_
#define _NRF24LIB_H_

/** ************************************************************************
 * Modulo: nrf24lib 
 * @file nrf24lib.h
 * @headerfile nrf24lib.h
 * @author Marcelo Martins Maia do Couto - Email: marcelo.m.maia@gmail.com
 * @date Apr 4, 2015
 *
 * @brief Substitua este texto por uma descrição breve deste módulo.
 *
 * Substitua este texto pela descrição completa deste módulo.
 * Este módulo é um modelo para a criação de novos módulos. Ele contém
 * toda a estrutura que um módulo deve conter, sendo composto pelos arquivos:
 *   - nrf24lib.c;
 *   - nrf24lib.h.
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

/*
 * Inclusão de arquivos de cabeçalho sem um arquivo ".c" correspondente.
 * Por exemplo: '#include "stddefs.h"'.
 */

/*
 * Inclusão de arquivos de cabeçalho de outros módulos utilizados por este.
 * Por exemplo: '#include "serial.h"'.
 */

/*******************************************************************************
 *                           DEFINICOES E MACROS							   *
 ******************************************************************************/
#define NRF24_MAX_CHANNEL 			125
#define NRF24_LENGTH_ADDRESS		5
#define NRF24_LENGTH_PAYLOAD		5

/*******************************************************************************
 *                     ESTRUTURAS E DEFINICOES DE TIPOS						   *	
 ******************************************************************************/
/**
 * NRF24 Radio Machine States.
 * For use with nrf24RadioControlState()
 */
typedef enum
{
	RF24_POWERDOWN = 0, /* Disable radio. */
	RF24_STANDBY_I,		/* Waiting to decide: Read or Write? */
	RF24_STANDBY_II,	/* Used for send data in burst. */
	RF24_RXMODE,		/* Reading RX Buffer. */
	RF24_TXMODE,		/* Reading TX Buffer. */
	RF24_WAITING,
} rf24_radio_states_e;

/**
 * Power Amplifier level.
 * For use with setPALevel()
 */
typedef enum
{
	RF24_PA_MINUS18DBM = 0,	/* -18dBm */
	RF24_PA_MINUS12DBM,		/* -12dBm */
	RF24_PA_MINUS6DBM,		/* -6dBm */
	RF24_PA_0DBM,			/* 0dBm */
} rf24_pa_e;

/**
 * Data rate.  How fast data moves through the air.
 * For use with setDataRate()
 */
typedef enum
{
	RF24_1MBPS = 0,
	RF24_2MBPS,
	RF24_250KBPS
} rf24_datarate_e;

/**
 * CRC Length.  How big (if any) of a CRC is included.
 * For use with setCRCLength()
 */
typedef enum
{
	RF24_CRC_DISABLED = 0,
	RF24_CRC_8,
	RF24_CRC_16
} rf24_crclength_e;

typedef struct
{
	bool cripto;
	uint8_t length;
	uint8_t buffer[NRF24_MAX_LENGTH_PAYLOAD];
} pipe_tx_t;

typedef struct
{
	bool cripto;
	uint8_t length;
	uint8_t buffer[NRF24_MAX_LENGTH_PAYLOAD];
} pipe_rx_t;

/*******************************************************************************
 *                       VARIAVEIS PUBLICAS (Globais)						   *
 ******************************************************************************/

/*******************************************************************************
 *                      PROTOTIPOS DAS FUNCOES PUBLICAS						   *
 ******************************************************************************/

/**
 * Inicialização deste módulo.
 *
 * @return void
 *
 * @pre Descreva a pré-condição para esta função
 *
 * @post Descreva a pós-condição para esta função
 *
 * @invariant Descreva o que não pode variar quando acabar a execução da função 
 *
 * @note
 *  Esta função deve ser chamada durante a inicialização do software caso este
 *  módulo seja utilizado.
 *
 * <b> Exemplo de uso: </b>
 * @code
 *    nrf24Init();
 * @endcode
 ******************************************************************************/
void nrf24Init(void);

/**
 * Test whether there was a carrier on the line for the
 * previous listening period.
 *
 * Useful to check for interference on the current channel.
 *
 * @return true if was carrier, false if not
 */
bool nrf24TestCarrier(void);

/**
 * Test whether a signal (carrier or otherwise) greater than
 * or equal to -64dBm is present on the channel. Valid only
 * on nRF24L01P (+) hardware. On nRF24L01, use testCarrier().
 *
 * Useful to check for interference on the current channel and
 * channel hopping strategies.
 *
 * @return true if signal => -64dBm, false if not
 */
bool nrf24TestRPD(void);

/**
 * Fetches the current PA level.
 *
 * @return Returns a value from the rf24_pa_dbm_e enum describing
 * the current PA setting. Please remember, all values represented
 * by the enum mnemonics are negative dBm. See setPALevel for
 * return value descriptions.
 */
rf24_pa_e nrf24GetPALevel(void);

/**
 * Fetches the transmission data rate
 *
 * @return Returns the hardware's currently configured datarate. The value
 * is one of 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS, as defined in the
 * rf24_datarate_e enum.
 */
rf24_datarate_e nrf24GetDataRate( void ) ;

/**
 * Set the CRC length
 *
 * @param length RF24_CRC_8 for 8-bit or RF24_CRC_16 for 16-bit
 */
void nrf24SetCRCLength(rf24_crclength_e length);

/**
 * Get the CRC length
 *
 * @return RF24_DISABLED if disabled or RF24_CRC_8 for 8-bit or RF24_CRC_16 for 16-bit
 */
rf24_crclength_e nrf24GetCRCLength(void);

/**
 * Disable CRC validation
 *
 */
void nrf24DisableCRC( void ) ;

///
void nrf24RadioControlState(void);
void nrf24ClearInterrupts(void);
void nrf24WriteTxPayload(uint8_t *buffer, uint8_t length);
void nrf24ReadRxPayload(uint8_t* buf, uint8_t len);
uint8_t nrf24GetStatus(void);
void nrf24FlushRx(void);
void nrf24FlushTx(void);
bool nrf24BuffersInit(bool cripto);
void nrf24SetMyAddress(uint8_t *address);

/*******************************************************************************
 *                                   EOF									   *	
 ******************************************************************************/
#endif
