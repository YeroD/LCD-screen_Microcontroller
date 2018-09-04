#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
          ______________________________________________________________________
LCD       ||__D7_|__D6_|__D5_|__D4_|__D3_|__D2_|__D1_|__D0_||_RS__|_R/W_|__E__||
Micro     ||__D8_|__D6_|_D12_|__D4_|__D1_|__D0_|__D2_|__D3_||_D9__|_D10_|_D11_||
Code      ||_PB4_|_PD7_|_PD6_|_PD4_|_PD3_|_PD2_|_PD1_|_PD0_||_PB5_|_PB6_|_PB7_||

________________________________________________________________________________

PORT D :
          ___________________________________________________
          ||_PD7_|_PD6_|_PD5_|_PD4_|_PD3_|_PD2_|_PD1_|_PD0_||
          ||_D6__|_D5__|__X__|_D4__|_D3__|_D2__|_D1__|_D0__||

________________________________________________________________________________

PORT B :
          ___________________________________________________
          ||_PB7_|_PB6_|_PB5_|_PB4_|_PB3_|_PB2_|_PB1_|_PB0_||
          ||__E__|_R/W__|_RS_|_D7__|__X__|__X__|__X__|__X__||
*/

void Send_A_Character(unsigned char character);
void Send_A_Command(unsigned char character);
void Send_a_String(char *string);
void Initialisation();

int main(void){
		  unsigned char data;
		  int i=0;
		  data = 0x00;
		  
          // Mettre en sortie les PORTB et  PORTD
          DDRD=0xFF;
          DDRB=0xFF ;
          Initialisation();
          /*40 Caractères dans une ligne, 16 caractères afficher*/
          Send_a_String("Votre Nom :");
          
		  
          
          while(1){
			  
			  if (((PINE)>>6)==1) {
				 
				 data |= (PINC & 0xC0);
				 data &= (PINC & 0xC0) | 0x3F;
				 
				 data |= (PINF & 0xF0) >> 2;
				 data &= ((PINF & 0xF0) >> 2) | 0xC3; 
				 
				 data |= (PINF & 0x03);
				 data &= (PINF & 0x03) | 0xFC;
	      			
	      			
	      		/*Si Data est égal à que des 1, on affiche le message de fin*/	
	      		if(data==0xFF || i==15){			      // Ou incrémentation d'un i dans le if qu'on fait pour limiter carac 
					Send_A_Command(0x01);
					Send_a_String("    Merci !");
					Send_a_String("  Au revoir !");
					while(1){
					}
				}else{
					Send_A_Character(data);
					i++;
				}
			
	       
	       _delay_ms(1000);
	       
	       }
			  
			  
          }

          return 0;
}


void Send_A_Command(unsigned char character){
          _delay_ms(100);
          PORTB =((PORTB<<8)|0x80);  /*On envoie E=1 avec RS et RW à 0, pour valider les entrées */
                                    /*Les entrées RS et RW sont lues sur le front montant
                                      Tandis que le bus de données, D0 à D7, est lu sur le front descendant*/
                                  /*R/W est toujours à 0, on est toujours en mode écriture et on n'utilise pas le mode lecture */
          _delay_ms(100);
          PORTD =((PORTD<<8)|character); /*On envoie les informations de D0 à D7*/
          _delay_ms(100);
          PORTB =((PORTB<<8)|0x00); /*On met E=0, pour les lire informations du bus de données
                                      RS=1, pour mettre à jour l'affichage */
}


void Send_A_Character(unsigned char character){
          PORTB = ((PORTB<<8)|0xA0);
          _delay_ms(100);

          PORTB |= (character&0x80)>>3;
          PORTB &=((character&0x80)>>3)|0xEF;

          PORTD |=(character&0x60)<<1;
          PORTD &=((character&0x60)<<1)|0x3F;

          PORTD |=(character&0x1F);
          PORTD &=((character&0x1F))|0xE0;

          _delay_ms(100);
          PORTB = ((PORTB<<8)|0x00);
}

void Initialisation(){
        _delay_ms(15);
        /**************Function set 1*******/
        Send_A_Command(0x58);
        /**************Function set 2*******/
        Send_A_Command(0x58);
        /**************Function set 3*******/
        Send_A_Command(0x58);
        /**************Function set 4*******/
        Send_A_Command(0x58);
        /**************Control Affichage************/
        Send_A_Command(0x0E);
        /************Display On/Off Control*********/
        Send_A_Command(0x0F);
        /************Clear Display******************/
        Send_A_Command(0x01);
        /************Entry Set Mode ****************/
        Send_A_Command(0x06);
}

void Send_a_String(char *string){
        int i;
        int j;
        int k;

       for(i=0;i<strlen(string);i++){
                     Send_A_Character(string[i]);
					/*Dès que la chaine est terminée ou si la chaine est inférieur
					 * à 16 caractères, saut à la ligne suivant*/
                     if (strlen(string)<=16 && i==(strlen(string)-1)){
                             k=24+(16-strlen(string));
                             for(j=0;j<k;j++){
                               Send_A_Character(0x20);
                             }
                     }

        }
}
