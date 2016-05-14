/** 
 *   Modulo: nrf24lib
 *   @file nrf24lib.c
 *   Veja nrf24lib.h para mais informações.
 ******************************************************************************/

/*******************************************************************************
 *                             MODULOS UTILIZADOS							   *
 ******************************************************************************/

/*
 * Inclusão de arquivos de cabeçalho da ferramenta de desenvolvimento.
 * Por exemplo: '#include <stdlib.h>'.
 */
#include <stdint.h>   /* Para as definições de uint8_t/uint16_t */
#include "stdio.h"
#include <stdbool.h>  /* Para as definições de true/false */ 

/*
 * Inclusão de arquivos de cabeçalho sem um arquivo ".c" correspondente.
 * Por exemplo: 
 * #include "stddefs.h" 
 * #include "template_header.h"
 */
#include "LPC17xx.h"
#include "nrf24l01.h"

/*
 * Inclusão de arquivos de cabeçalho de outros módulos utilizados por este.
 * Por exemplo: '#include "serial.h"'.
 */
#include "nrf24hal.h"
#include "nrf24cripto.h"

/*
 * Inclusão dos arquivos ".tab.h" deste módulo.
 * Por exemplo: 
 * #include "barcode.tab.h"
 * #include "template.tab.h"
 */

/*
 * Inclusão do arquivo de cabeçalho deste módulo.
 */
#include "nrf24lib.h"

/*******************************************************************************
 *                     CONSTANTES E DEFINICOES DE MACRO						   *
 ******************************************************************************/

/*******************************************************************************
 *                      ESTRUTURAS E DEFINIÇÕES DE TIPOS					   *
 ******************************************************************************/

/*******************************************************************************
 *                        VARIAVEIS PUBLICAS (Globais)						   *
 ******************************************************************************/
uint8_t state = RF24_POWERDOWN;
uint8_t myaddress[NRF24_LENGTH_ADDRESS] = {0x11,0x22,0x33,0x44,0x55};
uint8_t rxaddress[NRF24_LENGTH_ADDRESS] = {0xE7,0xE7,0xE7,0xE7,0xE7};

/*******************************************************************************
 *                  DECLARACOES DE VARIAVEIS PRIVADAS (static)				   *
 ******************************************************************************/
static pipe_tx_t txbuf;
static pipe_tx_t rxbuf[5];

/*******************************************************************************
 *                   PROTOTIPOS DAS FUNCOES PRIVADAS (static)				   *
 ******************************************************************************/

/*******************************************************************************
 *                      IMPLEMENTACAO DAS FUNCOES PUBLICAS					   *
 ******************************************************************************/

bool nrf24TestCarrier(void)
{
	uint8_t stat = 0;

	nrf24ReadRegister(CD, &stat);

	return (stat & 0x01);
}

/****************************************************************************/
bool nrf24TestRPD(void)
{
	uint8_t stat = 0;

	nrf24ReadRegister(RPD, &stat);

	return (stat & 0x01);
}

/****************************************************************************/
uint8_t nrf24GetStatus(void)
{
	return (nrf24WriteRegisterOnly(NOP));
}

/****************************************************************************/
rf24_pa_e nrf24GetPALevel(void)
{
	rf24_pa_e result;
	uint8_t power;

	nrf24ReadRegister(RF_SETUP, &power);

	power &= ((1<<RF_PWR_LOW) | (1<<RF_PWR_HIGH));

	/* switch uses RAM (evil!) */
	if ( power == ((1<<RF_PWR_LOW) | (1<<RF_PWR_HIGH)) )
	{
		result = RF24_PA_0DBM;
	}
	else if ( power == (1<<RF_PWR_HIGH) )
	{
		result = RF24_PA_MINUS6DBM;
	}
	else if ( power == (1<<RF_PWR_LOW) )
	{
		result = RF24_PA_MINUS12DBM;
	}
	else
	{
		result = RF24_PA_MINUS18DBM;
	}
	return (result);
}

/****************************************************************************/
rf24_datarate_e nrf24getDataRate( void )
{
	rf24_datarate_e result;
	uint8_t dr;

	nrf24ReadRegister(RF_SETUP, &dr);

	dr &= ((1<<RF_DR_LOW) | (1<<RF_DR_HIGH));

	/*  switch uses RAM (evil!) - Order matters in our case below */
	if ( dr == (1 << RF_DR_LOW) )
	{
		result = RF24_250KBPS;					// '10' = 250KBPS
	}
	else if ( dr == (1 << RF_DR_HIGH) )
	{
		result = RF24_2MBPS;					// '01' = 2MBPS
	}
	else
	{
		result = RF24_1MBPS;					// '00' = 1MBPS
	}
	return result;
}

/****************************************************************************/
void nrf24SetCRCLength(rf24_crclength_e length)
{
	uint8_t config;

	nrf24ReadRegister(CONFIG, &config);

	config &= ~((1 << CRCO) | (1 << EN_CRC));

	if (length == RF24_CRC_16)
	{
		config |= (1 << EN_CRC);
		config |= (1 << CRCO);
	}
	else if ( length == RF24_CRC_8 )
	{
		config |= (1 << EN_CRC);
	}

	nrf24WriteRegister(CONFIG, &config);
}

/****************************************************************************/
rf24_crclength_e nrf24GetCRCLength(void)
{
	rf24_crclength_e result = RF24_CRC_DISABLED;
	uint8_t config;

	nrf24ReadRegister(CONFIG, &config);

	config &= ((1<<CRCO) | (1<<EN_CRC));
	if (config & (1 << EN_CRC))
	{
		if (config & (1 << CRCO))
		{
			result = RF24_CRC_16;
		}
		else
		{
			result = RF24_CRC_8;
		}
	}
	return (result);
}

/****************************************************************************/
void nrf24DisableCRC(void)
{
	uint8_t disable;

	nrf24ReadRegister(CONFIG, &disable);

	disable &= ~(1 << EN_CRC);

	nrf24WriteRegister(CONFIG, &disable);
}

/****************************************************************************
**Auto Retransmit Delay
	‘0000’ – Wait 250μS
	‘0001’ – Wait 500μS
	‘0010’ – Wait 750μS
	........
	‘1111’ – Wait 4000μS
	(Delay defined from end of transmission to start of	next transmission)

**Auto Retransmit Count
	‘0000’ –Re-Transmit disabled
	‘0001’ – Up to 1 Re-Transmit on fail of AA
	......
	‘1111’ – Up to 15 Re-Transmit on fail of AA
*/
void nrf24SetAutoRetries(uint8_t delay, uint8_t count)
{
	uint8_t tried = 0;

	tried = ((delay & 0xf) << ARD) | ((count & 0xf) << ARC);

	nrf24WriteRegister(SETUP_RETR, &tried);
}

/****************************************************************************/
void nrf24FlushRx(void)
{
	nrf24WriteRegisterOnly(FLUSH_RX);
}

/****************************************************************************/
void nrf24FlushTx(void)
{
	nrf24WriteRegisterOnly(FLUSH_TX);
}

/****************************************************************************/
void nrf24SetChannel(uint8_t channel)
{
	uint8_t checkchannel;

	checkchannel = channel > NRF24_MAX_CHANNEL ? NRF24_MAX_CHANNEL : channel;

	nrf24WriteRegister(RF_CH, &checkchannel);	/* ChFreq = 2400 + RF_CH[MHz] */
}

/****************************************************************************/
void nrf24PowerDown(void)
{
	uint8_t cfg;

	nrf24ReadRegister(CONFIG, &cfg);
	cfg &= ~(1 << PWR_UP);

	nrf24WriteRegister(CONFIG, &cfg);
}

/****************************************************************************/
void nrf24PowerUp(void)
{
	uint8_t cfg;

	nrf24ReadRegister(CONFIG, &cfg);
	cfg |= (1<<PWR_UP);
	nrf24WriteRegister(CONFIG, &cfg);
}

/****************************************************************************/
bool nrf24EnablePipeAutoAck(uint8_t pipe, bool enable)
{
	uint8_t en_aa;

	if (pipe < 6)
	{
		nrf24ReadRegister(EN_AA, &en_aa);
		if(enable)
		{
			en_aa |= (1 << pipe);
		}
		else
		{
			en_aa &= ~(1 << pipe);
		}
		nrf24WriteRegister(EN_AA, &en_aa);
		return(true);
	}
	else if (pipe == 0xff)
	{
		en_aa = 0x3f;
		nrf24WriteRegister(EN_AA, &en_aa);
		return(true);
	}
	return (false);
}

/****************************************************************************/
bool nrf24EnableRxDataPipe(uint8_t pipe, bool enable)
{
	uint8_t en_rxaddr;

	if (pipe < 6)
	{
		nrf24ReadRegister(EN_RXADDR, &en_rxaddr);
		if(enable)
		{
			en_rxaddr |= (1 << pipe);
		}
		else
		{
			en_rxaddr &= ~(1 << pipe);
		}
		nrf24WriteRegister(EN_RXADDR, &en_rxaddr);
		return(true);
	}
	else if (pipe == 0xff)
	{
		en_rxaddr = 0x3f;
		nrf24WriteRegister(EN_RXADDR, &en_rxaddr);
		return(true);
	}
	return (false);
}

/****************************************************************************/
bool nrf24SetAddressLength(uint8_t length)
{
	if (length > 2 || length < 6)
	{
		length -= 2;
		nrf24WriteRegister(SETUP_AW, &length);
		return(true);
	}
	return (false);
}

/****************************************************************************/
uint8_t nrf24GetAddressLength(void)
{
	uint8_t addrlen;

	nrf24ReadRegister(SETUP_AW, &addrlen);

	return (addrlen + 2);
}

/****************************************************************************/
bool nrf24SetRfSetup(bool contwave, rf24_datarate_e rate, rf24_pa_e outpower)
{
	uint8_t setup = 0;

	if(contwave)
	{
		setup |= (1<<CONT_WAVE);
	}

	if(rate == RF24_250KBPS)
	{
		setup |= (1<<RF_DR_LOW);
	}
	else if (rate == RF24_2MBPS)
	{
		setup |= (1<<RF_DR_HIGH);
	}
	setup |= outpower;

	nrf24WriteRegister(RF_SETUP, &setup);

	return(true);
}

/****************************************************************************/
void nrf24PrintStatusRegister(void)
{
	uint8_t status;

	status = nrf24WriteRegisterOnly(NOP);

	printf("\nStatus=0x%02x Rx_dr=%x Tx_ds=%x Max_rt=%x Rx_p_no=%x Tx_full=%x\r\n",
			status,
           (status & (1 << RX_DR))?1:0,
           (status & (1 << TX_DS))?1:0,
           (status & (1 << MAX_RT))?1:0,
           ((status >> RX_P_NO) & 0b111),
           (status & (1 << TX_FULL))?1:0);
}

/****************************************************************************/
void nrf24PrintByteRegister(const uint8_t *name, uint8_t reg)
{
	uint8_t value;

	printf("\t%s =", name);
	nrf24ReadRegister(reg, &value);
	printf(" 0x%02x \r\n", value);
}

/****************************************************************************/
bool nrf24IsDataSentInterrupt(void)
{
	uint8_t status;

	status = nrf24WriteRegisterOnly(NOP);

	if(status & (1 << TX_DS))
	{
		status |= (1 << TX_DS);
		nrf24WriteRegister(STATUS, &status);
		return (true);
	}
	return (false);
}


/****************************************************************************/
bool nrf24IsDataReceivedInterrupt(void)
{
	uint8_t status;

	status = nrf24WriteRegisterOnly(NOP);

	if(status & (1 << RX_DR))
	{
		status |= (1 << RX_DR);
		nrf24WriteRegister(STATUS, &status);
		return (true);
	}
	return (false);
}

/****************************************************************************/
bool nrf24IsMaxRetransmitInterrupt(void)
{
	uint8_t status;

	status = nrf24WriteRegisterOnly(NOP);

	if(status & (1 << MAX_RT))
	{
		status |= (1 << MAX_RT);
		nrf24WriteRegister(STATUS, &status);
		return (true);
	}
	return (false);
}

/****************************************************************************/
void nrf24ClearInterrupts(void)
{
	uint8_t status;

	status = nrf24GetStatus();
	status |= ((1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT));

	nrf24WriteRegister(STATUS, &status);
}

/****************************************************************************/
void nrf24SetRxAddressPipe0(uint8_t* address)
{
	nrf24WriteRegisterBuf(RX_ADDR_P0, address, nrf24GetAddressLength());
}

/****************************************************************************/
void nrf24SetRxAddressPipe1(uint8_t* address)
{
	nrf24WriteRegisterBuf(RX_ADDR_P1, address, nrf24GetAddressLength());
}

/****************************************************************************/
bool nrf24SetRxAddressPipe2to5(uint8_t pipe, uint8_t address)
{
	if((pipe > 1) && (pipe < 6))
	{
		nrf24WriteRegister((RX_ADDR_P0 + pipe), &address);
		return (true);
	}
	return (false);
}

/****************************************************************************/
void nrf24SetMyAddress(uint8_t *address)
{
	uint8_t stat;

	nrf24WriteRegisterBuf(TX_ADDR, address, NRF24_LENGTH_ADDRESS);
	nrf24ReadRegister(EN_AA, &stat);

	/* If Enhanced Shockburst enabled, we have to setup RX_ADDR_P0 with same address because automatic acknowledge will validate! */
	if(stat & (1 << ENAA_P0))
	{
		/* If ACK enable, TXAddr is the same that RXAddr in pipe 0 */
		nrf24WriteRegisterBuf(RX_ADDR_P0, address, NRF24_LENGTH_ADDRESS);
		stat = (1 << ERX_P0);
		nrf24WriteRegister(EN_RXADDR, &stat);
	}
}

/****************************************************************************/
void nrf24SetAddressPipe1(uint8_t *address)
{
	nrf24WriteRegisterBuf(RX_ADDR_P1, address, NRF24_LENGTH_ADDRESS);
}

/****************************************************************************/
void nrf24ClearRfBuffers(void)
{
	ce(LOW);

	/* Flush buffers */
	nrf24FlushRx();
	nrf24FlushTx();
}

/****************************************************************************/
uint8_t nrf24GetPayloadRxSize(uint8_t pipe)
{
	uint8_t pload = 0;

	if(pipe < 6)
	{
		nrf24ReadRegister((RX_PW_P0 + pipe), &pload);
	}

	return (pload);
}

/****************************************************************************/
bool nrf24SetPayloadRxSize(uint8_t pipe, uint8_t length)
{
	if((pipe < 6) && (length < 32))
	{
		nrf24WriteRegister((RX_PW_P0 + pipe), &length);
		return (true);
	}
	return (false);
}

/****************************************************************************/
bool nrf24GetTxReuseFlag(void)
{
	uint8_t fifo_status;

	nrf24ReadRegister(FIFO_STATUS, &fifo_status);

	if(fifo_status & (1 << TX_REUSE))
	{
		return (true);
	}
	return (false);
}

/****************************************************************************/
bool nrf24GetTxFullFlag(void)
{
	uint8_t status;

	status = nrf24WriteRegisterOnly(NOP);

	if(status & (1<<TX_FULL))
	{
		return (true);
	}
	return (false);
}

/****************************************************************************/
bool nrf24GetTxEmptyFlag(void)
{
	uint8_t fifo_status;

	nrf24ReadRegister(FIFO_STATUS, &fifo_status);

	if(fifo_status & (1 << TX_EMPTY))
	{
		return (true);
	}
	return (false);
}

/****************************************************************************/
bool nrf24GetRxFullFlag(void)
{
	uint8_t fifo_status;

	nrf24ReadRegister(FIFO_STATUS, &fifo_status);

	if(fifo_status & (1<<RX_FULL))
	{
		return (true);
	}
	return (false);
}

/****************************************************************************/
bool nrf24GetRxEmptyFlag(void)
{
	uint8_t fifo_status;

	nrf24ReadRegister(FIFO_STATUS, &fifo_status);

	if(fifo_status & (1<<RX_EMPTY))
	{
		return (true);
	}
	return (false);
}

/****************************************************************************/
uint8_t nrf24GetRetransmissionCount(void)
{
	uint8_t info;

	nrf24ReadRegister(OBSERVE_TX, &info);

	info &= 0x0f;

	printf("# Retransmission Count: %d \r\n", info);

	return (info);
}

/****************************************************************************/
bool nrf24SetDinamicPayloadLength(uint8_t pipe)
{
	uint8_t tmp = 0;

	if(pipe < 6)
	{
		tmp = (1 << EN_DPL);
		nrf24WriteRegister(FEATURE, &tmp);

		if(nrf24EnablePipeAutoAck(pipe, true))
		{
			tmp = (1 << pipe);
			nrf24WriteRegister(DYNPD, &tmp);
		}

		return (true);
	}
	return (false);
}

/****************************************************************************/
void nrf24SetFeatures(bool dynamic, bool payloadwithack, bool payloadwithoutack)
{
	uint8_t feature = 0;

	feature |= ((1<<EN_DPL) | (1<<EN_ACK_PAY) | (1<<EN_DYN_ACK));

	nrf24WriteRegister(FEATURE, &feature);
}

/****************************************************************************/
uint8_t nrf24GetRxPayloadAvaliable(void)
{
	uint8_t status;
    uint8_t result = 0x07;

    status = nrf24GetStatus();
	if (status & (1 << RX_DR))
	{
		/* Checking what's pipe with RX Avaliable */
		result = status & 0x0e;

		/* Checking if AutoAck is enabled */
		nrf24ReadRegister(EN_AA, &status);
		if(status & (1 << result))
		{
			/* Data Sent TX FIFO interrupt. Asserted when packet transmitted on TX.
			 * If AUTO_ACK is activated, this bit is set high only when ACK is
			 * received. Write 1 to clear bit.
			 */
			if (status & (1 << TX_DS))
			{
				status = (1 << TX_DS);
				nrf24WriteRegister(STATUS, &status);
			}
		}
	}
	return (result);
}

/****************************************************************************/
void nrf24ReadRxPayload(uint8_t* buf, uint8_t len)
{
	nrf24ReadPayload(buf, len);
}

/****************************************************************************/
void nrf24WriteTxPayload(uint8_t *buffer, uint8_t length)
{
	uint8_t datalength;

	datalength = length > NRF24_MAX_LENGTH_PAYLOAD ? NRF24_MAX_LENGTH_PAYLOAD : length;
	nrf24WritePayload(buffer, datalength);
}

/****************************************************************************/
void nrf24AnalysingChannels(void)
{
	uint8_t idxtmp = 0;
	uint8_t tmp = 0;

	/* Putting nrf24l01+ in RX mode */
	nrf24ReadRegister(CONFIG, &tmp);
	tmp |= (1 << PRIM_RX);
	nrf24WriteRegister(CONFIG, &tmp);
	delayUs(200);
	do
	{
		nrf24SetChannel(idxtmp);
		ce(HIGH);
		delayUs(200);
		nrf24ReadRegister(RPD, &tmp);
		printf("Channel: %d - RDP: %d \t\n", idxtmp, tmp);
		ce(LOW);
		idxtmp++;
	} while (idxtmp < NRF24_MAX_CHANNEL);
}

/****************************************************************************/
void nrf24Init(void)
{
	bool cripto = false;

	printf("\nnRF24l01+ Stack for LPC1768.");
	printf("\nCopyright @ M3C Tecnologia.");
	printf("\nBy Marcelo Martins Maia do Couto.");
	printf("\nCompiled in %s at %s.\n", __DATE__, __TIME__);

	if(nrf24HalInit())									/* Init SPI, CSN and CE */
	{
		printf("\nHAL: SPI port enabled.");
	}

	if(nrf24BuffersInit(cripto))						/* Init buffers in memory */
	{
		printf("\nBUF: Initialized.");
	}

	if(cripto == true)
	{
		nrf24criptoInitKSA();							/* Criptography data */
		printf("\nCRPT: Initialized.");
	}

	nrf24PowerUp();

	nrf24AnalysingChannels();							/* Analysing Channels. */
	nrf24PrintStatusRegister();							/* Printing stats. */

	nrf24SetRfSetup(false, RF24_1MBPS, RF24_PA_0DBM);	/* Configure radio parameters first. */
	nrf24SetChannel(1);									/* Choosing channel. */
	nrf24SetCRCLength(RF24_CRC_8);						/* Configure Enhanced Shockburst - CRC Length. */
	nrf24SetAutoRetries(0x04, 0x04);					/* Configure Enhanced Shockburst - AutoRetransmit 4x with interval 1ms. */
	nrf24SetAddressLength(NRF24_LENGTH_ADDRESS);		/* Configure Enhanced Shockburst - Address using addresslength bytes. */
	nrf24EnablePipeAutoAck(0, true);					/* Configure Enhanced Shockburst - Auto Ack to Pipe 0. */
	nrf24EnablePipeAutoAck(1, true);					/* Configure Enhanced Shockburst - Auto Ack to Pipe 1. */
	nrf24EnableRxDataPipe(0, true);						/* Configure Enhanced Shockburst - Enable Pipe Rx 0 . */
	nrf24EnableRxDataPipe(1, true);						/* Configure Enhanced Shockburst - Enable Pipe Rx 1 . */
	nrf24SetPayloadRxSize(0, NRF24_LENGTH_PAYLOAD);		/* Setting payload length to receive from TX - The same payload length has to be sent. */
	nrf24SetPayloadRxSize(1, NRF24_LENGTH_PAYLOAD);		/* Setting payload length to receive from TX - The same payload length has to be sent. */

	nrf24SetMyAddress(myaddress);						/* Configuring My Address - Setting address to TXPIPE and RXPIPE0. */
	nrf24SetAddressPipe1(rxaddress);					/* Configuring Address to RXPIPE1. */

	delayUs(1500);
}

/****************************************************************************/
void nrf24RadioControlState(void)
{
	uint8_t pipenumber = 0;
	uint8_t tmp = 0;

	/* Aqui vamos desconsiderar o modo de Standby-II por enquanto.
	 * Será importante com o streammer de dados de video e audio */

	switch(state)
	{
		case RF24_STANDBY_I:
			ce(LOW);
			if(txbuf.length > 0)
			{
				if(txbuf.cripto == true)
				{
					nrf24criptoEncrypt(txbuf.buffer, txbuf.length);
				}
				nrf24ReadRegister(CONFIG, &tmp);
				tmp &= ~(1 << PRIM_RX);
				nrf24WriteRegister(CONFIG, &tmp);

				nrf24WriteTxPayload(txbuf.buffer, txbuf.length);						/* Send data to transmit */
				txbuf.length = 0;														/* Transmitido. */
				state = RF24_TXMODE;
			}
			else
			{
				nrf24ReadRegister(CONFIG, &tmp);
				tmp |= (1 << PRIM_RX);
				nrf24WriteRegister(CONFIG, &tmp);
				state = RF24_RXMODE;
			}
		break;
		case RF24_TXMODE:
			ce(HIGH);
			delayUs(10);																	/* Datasheet recomendation. */
			if(nrf24IsDataSentInterrupt())													/* Bit activation after ACK only. */
			{
				if(nrf24IsDataReceivedInterrupt())
				{
					pipenumber = nrf24GetStatus() & (0x07 << RX_P_NO);
					nrf24ReadPayload(rxbuf[pipenumber].buffer, rxbuf[pipenumber].length);	/* Read Payload from Pipe. */
					if(rxbuf[pipenumber].cripto == true)
					{
						nrf24criptoDecrypt(rxbuf[pipenumber].buffer, rxbuf[pipenumber].length);
					}
				}
				/* If we have data to send... Go ON! -> STANDBY_II */
				if(txbuf.length == 0)
				{
					state = RF24_STANDBY_I;
				}
				else
				{
					if(txbuf.cripto == true)
					{
						nrf24criptoEncrypt(txbuf.buffer, txbuf.length);
					}
					nrf24ReadRegister(CONFIG, &tmp);
					tmp &= ~(1 << PRIM_RX);
					nrf24WriteRegister(CONFIG, &tmp);
					nrf24WriteTxPayload(txbuf.buffer, txbuf.length);						/* Send data to transmit */
					txbuf.length = 0;														/* Transmitido. */
				}
			}
			else if(nrf24IsMaxRetransmitInterrupt())
			{
				txbuf.length = 0;
				nrf24FlushTx();
				nrf24ClearInterrupts();
				state = RF24_STANDBY_I;
				/* Error Exception. */
			}
		break;
		case RF24_RXMODE:
			ce(HIGH);
			delayUs(130);	/* Datasheet recomendation. */
			if(nrf24IsDataReceivedInterrupt())
			{
				ce(LOW);																	/* Recebido. */
				pipenumber = nrf24GetStatus() & (0x07 << RX_P_NO);							/* Getting pipe in buffer RX full. */
				nrf24ReadPayload(rxbuf[pipenumber].buffer, nrf24GetPayloadRxSize(pipenumber));
				if(nrf24IsDataSentInterrupt())
				{
					nrf24ReadPayload(txbuf.buffer, txbuf.length);							/* Read Payload from Pipe. */
				}
			}
			if(txbuf.length > 0)
			{
				state = RF24_STANDBY_I;
			}
		break;
		case RF24_STANDBY_II:

		break;
		default:
		case RF24_POWERDOWN:
			nrf24PowerUp();			/* Start me up! */
			nrf24ClearRfBuffers();	/* Cleaning pipes */
			state = RF24_STANDBY_I;
		break;
	}
}

/****************************************************************************/
bool nrf24BuffersInit(bool cripto)
{
	uint8_t idx;

	txbuf.cripto = cripto;
	for (idx = 0; idx < 5; idx++)
	{
		rxbuf[idx].cripto = false;
		rxbuf[idx].length = 0;
	}

	txbuf.length = 0;

	for(idx = 0; idx < NRF24_MAX_LENGTH_PAYLOAD; idx++)
	{
		txbuf.buffer[idx] = 0;
		rxbuf[0].buffer[idx] = 0;
		rxbuf[1].buffer[idx] = 0;
		rxbuf[2].buffer[idx] = 0;
		rxbuf[3].buffer[idx] = 0;
		rxbuf[4].buffer[idx] = 0;
	}

	return(true);
}


/******************************************************************************
 *                    IMPLEMENTACAO DAS FUNCOES PRIVADAS					  *
 *****************************************************************************/

/******************************************************************************	
 *                                    EOF                                     *
 *****************************************************************************/
