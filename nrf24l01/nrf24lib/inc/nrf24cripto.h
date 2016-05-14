#ifndef _NRF24CRIPTO_H_
#define _NRF24CRIPTO_H_

/** ************************************************************************
 * Modulo: nrf24cripto 
 * @file nrf24cripto.h
 * @headerfile nrf24cripto.h
 * @author Marcelo Martins Maia do Couto - Email: marcelo.m.maia@gmail.com
 * @date Apr 4, 2015
 *
 * @brief Substitua este texto por uma descrição breve deste módulo.
 *
 * Substitua este texto pela descrição completa deste módulo.
 * Este módulo é um modelo para a criação de novos módulos. Ele contém
 * toda a estrutura que um módulo deve conter, sendo composto pelos arquivos:
 *   - nrf24cripto.c;
 *   - nrf24cripto.h.
 *
 * @copyright Copyright 2015 M3C Tecnologia
 * @copyright Todos os direitos reservados.
 *
 * @note
 *  - Baseado/copiado dos exemplos em: http://blog.diyembedded.com/
 *  - Arquivo original: arc4.h/arc4.c
 *  - Copyright Original: Copyright S. Brennen Ball, 2007
 *  - E-mail de contato: brennen@diyembedded.com
 *  - Disclaimer Original:
 * 		The author provides no guarantees, warantees, or promises, implied
 * 		or otherwise.  By using this software you agree to indemnify the
 * 		author of any damages incurred by using it.
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
#include "LPC17xx.h"

/*
 * Inclusão de arquivos de cabeçalho de outros módulos utilizados por este.
 * Por exemplo: '#include "serial.h"'.
 */

/*******************************************************************************
 *                           DEFINICOES E MACROS							   *
 ******************************************************************************/
#define NRF24CRIPTOLENGTHKEY	10		/* Tamanho da chave inicial KSA */

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
 *    template_init();
 * @endcode
 ******************************************************************************/
void nrf24criptoInitKSA(void);
uint8_t nrf24criptoGetBytePRGA(void);
void nrf24criptoEncrypt(uint8_t * buffer, uint32_t bufflen);
void nrf24criptoDecrypt(uint8_t * buffer, uint32_t bufflen);

/*******************************************************************************
 *                                   EOF									   *	
 ******************************************************************************/
#endif
