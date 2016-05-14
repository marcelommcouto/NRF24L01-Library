/** 
 *   Modulo: nrf24cripto
 *   @file nrf24cripto.c
 *   Veja nrf24cripto.h para mais informações.
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

/*
 * Inclusão de arquivos de cabeçalho de outros módulos utilizados por este.
 * Por exemplo: '#include "serial.h"'.
 */

/*
 * Inclusão dos arquivos ".tab.h" deste módulo.
 * Por exemplo: 
 * #include "barcode.tab.h"
 * #include "template.tab.h"
 */

/*
 * Inclusão do arquivo de cabeçalho deste módulo.
 */
#include "nrf24cripto.h"

/*******************************************************************************
 *                     CONSTANTES E DEFINICOES DE MACRO						   *
 ******************************************************************************/

/*******************************************************************************
 *                      ESTRUTURAS E DEFINIÇÕES DE TIPOS					   *
 ******************************************************************************/

/*******************************************************************************
 *                        VARIÁVEIS PUBLICAS (Globais)						   *
 ******************************************************************************/
uint8_t mycriptokey[NRF24CRIPTOLENGTHKEY] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
uint8_t strcripto[256];		/* S array */
uint32_t a = 0;				/* a array index */
uint32_t b = 0;				/* b array index */

/*******************************************************************************
 *                  DECLARACOES DE VARIAVEIS PRIVADAS (static)				   *
 ******************************************************************************/

/*******************************************************************************
 *                   PROTOTIPOS DAS FUNCOES PRIVADAS (static)				   *
 ******************************************************************************/

/*******************************************************************************
 *                      IMPLEMENTACAO DAS FUNCOES PUBLICAS					   *
 ******************************************************************************/
/*******************************************************************************
 * void nrf24criptoInitKSA (void)
 *
 * Description:
 *  Execute the KSA on the S array.
 *
 * Parameters:
 *  None
 *
 * Return value:
 *  None
*******************************************************************************/
void nrf24criptoInitKSA(void)
{
	uint32_t i, j = 0;
	uint8_t temp;

    a = 0;
    b = 0;

    for (i = 0; i < 256; i++) strcripto[i] = (uint8_t)i;

    for (i = 0; i < 256; i++)
    {
        j = (j + strcripto[i] + mycriptokey[i % NRF24CRIPTOLENGTHKEY]) % 256;
        temp = strcripto[i];
        strcripto[i] = strcripto[j];
        strcripto[j] = temp;
    }
}


/*******************************************************************************
 * uint8_t nrf24criptoGetBytePRGA(void)
 *
 * Description:
 *  Get the next byte from the PRGA and update S.
 *
 * Parameters:
 *  none
 *
 * Return value:
 *  The next byte from the PRGA
*******************************************************************************/
uint8_t nrf24criptoGetBytePRGA(void)
{
	uint8_t temp;

    a = (a + 1) % 256;
    b = (b + strcripto[a]) % 256;

    temp = strcripto[a];
    strcripto[a] = strcripto[b];
    strcripto[b] = temp;

    return strcripto[(strcripto[a] + strcripto[b]) % 256];
}

/******************************************************************************
 * void nrf24criptoEncrypt(uint8_t * buffer, uint32_t bufflen)
 *
 * Description:
 *  Encrypt an array of bytes.
 *
 * Parameters:
 *  uint8_t * buffer - Array of bytes that holds the data to be encrypted.
 *   This data is overwritten in the algorithm with encrypted data.
 *  uint32_t bufflen - Length of the buffer.
 *
 * Return value:
 *  None
*******************************************************************************/
void nrf24criptoEncrypt(uint8_t * buffer, uint32_t bufflen)
{
	uint32_t c;

    for (c = 0; c < bufflen; c++)
        buffer[c] = buffer[c] ^ nrf24criptoGetBytePRGA();
}

/******************************************************************************
 * void nrf24criptoDecrypt(uint8_t * buffer, uint32_t bufflen)
 *
 * Description:
 *  Decrypt an array of bytes.
 *
 * Parameters:
 *  uint8_t * buffer - Array of bytes that holds the data to be decrypted.
 *   This data is overwritten in the algorithm with decrypted data.
 *  uint32_t bufflen - Length of the buffer.
 *
 * Return value:
 *  None
*******************************************************************************/
void nrf24criptoDecrypt(uint8_t * buffer, uint32_t bufflen)
{
	nrf24criptoEncrypt(buffer, bufflen);
}

/******************************************************************************
 *                    IMPLEMENTACAO DAS FUNCOES PRIVADAS					  *
 *****************************************************************************/

/******************************************************************************	
 *                                    EOF                                     *
 *****************************************************************************/
