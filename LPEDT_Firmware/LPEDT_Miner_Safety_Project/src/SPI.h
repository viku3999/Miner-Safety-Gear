/*
 * SPI.h
 *
 *  Created on: Oct 12, 2024
 *      Author: vishn
 */

#ifndef SRC_SPI_H_
#define SRC_SPI_H_

#define TX_BUFFER_SIZE   1
#define RX_BUFFER_SIZE   TX_BUFFER_SIZE


void initUSART0();

void Send_tx(void);

void SPI_Trial_Init();

void SPI_Trial();

#endif /* SRC_SPI_H_ */
