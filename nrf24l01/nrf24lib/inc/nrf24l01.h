#ifndef _NRF24L01_H_
#define _NRF24L01_H_

/** ************************************************************************
 * Modulo: nrf24l01 
 * @file nrf24l01.h
 * @headerfile nrf24l01.h
 * @author Marcelo Martins Maia do Couto - Email: marcelo.m.maia@gmail.com
 * @date Apr 4, 2015
 *
 * @brief Substitua este texto por uma descrição breve deste módulo.
 *
 * Substitua este texto pela descrição completa deste módulo.
 * Este módulo é um modelo para a criação de novos módulos. Ele contém
 * toda a estrutura que um módulo deve conter, sendo composto pelos arquivos:
 *   - nrf24l01.c;
 *   - nrf24l01.h.
 *
 * @copyright Copyright 2015 M3C Tecnologia
 * @copyright Todos os direitos reservados.
 *
  * @note
 *  - Baseado/copiado dos exemplos em: https://github.com/maniacbug/RF24
 *  - Arquivo original: nrf24l01.h
 *  - Copyright Original: Copyright (c) 2007 Stefan Engelke
 *  - E-mail de contato: mbox@stefanengelke.de
 *  - Disclaimer Original:
 * 		Permission is hereby granted, free of charge, to any person
 *  	obtaining a copy of this software and associated documentation
 *  	files (the "Software"), to deal in the Software without
 *  	restriction, including without limitation the rights to use, copy,
 *  	modify, merge, publish, distribute, sublicense, and/or sell copies
 *  	of the Software, and to permit persons to whom the Software is
 *  	furnished to do so, subject to the following conditions:
 *
 *  	The above copyright notice and this permission notice shall be
 *  	included in all copies or substantial portions of the Software.
 *
 *  	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  	DEALINGS IN THE SOFTWARE.
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
/* Memory Map */
#define CONFIG      	0x00
#define EN_AA       	0x01
#define EN_RXADDR   	0x02
#define SETUP_AW    	0x03
#define SETUP_RETR  	0x04
#define RF_CH       	0x05
#define RF_SETUP    	0x06
#define STATUS      	0x07
#define OBSERVE_TX  	0x08
#define CD          	0x09
#define RX_ADDR_P0  	0x0A
#define RX_ADDR_P1  	0x0B
#define RX_ADDR_P2  	0x0C
#define RX_ADDR_P3 		0x0D
#define RX_ADDR_P4  	0x0E
#define RX_ADDR_P5  	0x0F
#define TX_ADDR     	0x10
#define RX_PW_P0    	0x11
#define RX_PW_P1    	0x12
#define RX_PW_P2    	0x13
#define RX_PW_P3   		0x14
#define RX_PW_P4    	0x15
#define RX_PW_P5    	0x16
#define FIFO_STATUS 	0x17
#define DYNPD	    	0x1C
#define FEATURE	    	0x1D

/* Bit Mnemonics */
#define MASK_RX_DR  	6
#define MASK_TX_DS  	5
#define MASK_MAX_RT 	4
#define EN_CRC      	3
#define CRCO        	2
#define PWR_UP      	1
#define PRIM_RX     	0
#define ENAA_P5     	5
#define ENAA_P4     	4
#define ENAA_P3     	3
#define ENAA_P2     	2
#define ENAA_P1     	1
#define ENAA_P0     	0
#define ERX_P5      	5
#define ERX_P4      	4
#define ERX_P3      	3
#define ERX_P2      	2
#define ERX_P1      	1
#define ERX_P0      	0
#define AW          	0
#define ARD         	4
#define ARC         	0
#define PLL_LOCK    	4
#define RF_DR       	3
#define RF_PWR      	6
#define RX_DR       	6
#define TX_DS       	5
#define MAX_RT      	4
#define RX_P_NO     	1
#define TX_FULL     	0
#define PLOS_CNT    	4
#define ARC_CNT     	0
#define TX_REUSE    	6
#define FIFO_FULL   	5
#define TX_EMPTY    	4
#define RX_FULL    	 	1
#define RX_EMPTY    	0
#define DPL_P5	    	5
#define DPL_P4	    	4
#define DPL_P3	    	3
#define DPL_P2	    	2
#define DPL_P1	    	1
#define DPL_P0	    	0
#define EN_DPL	    	2
#define EN_ACK_PAY  	1
#define EN_DYN_ACK  	0
#define CONT_WAVE		7

/* Instruction Mnemonics */
#define R_REGISTER    	0x00
#define W_REGISTER    	0x20
#define REGISTER_MASK 	0x1F
#define ACTIVATE      	0x50
#define R_RX_PL_WID   	0x60
#define R_RX_PAYLOAD  	0x61
#define W_TX_PAYLOAD  	0xA0
#define W_ACK_PAYLOAD 	0xA8
#define FLUSH_TX      	0xE1
#define FLUSH_RX      	0xE2
#define REUSE_TX_PL   	0xE3
#define NOP           	0xFF

/* Non-P omissions */
#define LNA_HCURR   	0

/* P model memory Map */
#define RPD         0x09

/* P model bit Mnemonics */
#define RF_DR_LOW   5
#define RF_DR_HIGH  3
#define RF_PWR_LOW  1
#define RF_PWR_HIGH 2


/*******************************************************************************
 *                     ESTRUTURAS E DEFINICOES DE TIPOS						   *	
 ******************************************************************************/

/*******************************************************************************
 *                       VARIAVEIS PUBLICAS (Globais)						   *
 ******************************************************************************/

/*******************************************************************************
 *                      PROTOTIPOS DAS FUNCOES PUBLICAS						   *
 ******************************************************************************/

/*******************************************************************************
 *                                   EOF									   *	
 ******************************************************************************/
#endif
