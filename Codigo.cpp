#include <18F4550.h> //Inclui biblioteca do microcontrolador PIC 18F4550
#include <18F4550.h> //Inclui biblioteca do microcontrolador PIC 18F4550
#device PASS_STRINGS = IN_RAM //Habilita o uso de ponteiros para alocação de memoria usado na biblioteca SSD1306OLED.c
#use delay(clock=48000000) //Define clock de trabalho
#use I2C(MASTER, I2C1, FAST = 400000, stream = SSD1306_STREAM)  // Initializa comunicação I2C
#include <SSD1306OLED.c> //Inclui biblioteca do display Oled
#include <KBD4X4.c>  //Inclui biblioteca do teclado matricial 4x4
#use standard_io(B) //Possibilita que porta B seja entrada ou saída dependendo somente do comando de Input ou Output
#use fast_io(B)   //Possibilita pulso rápidos na porta B


void main(void) {

  SSD1306_Begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS); //Inicialize o SSD1306 OLED com um addr I2C = 0x7A (endereço padrão)
  SSD1306_ClearDisplay (); //Limpa Display
  PORT_D_PULLUPS (0XFF); //Permite que as entradas sejam botões e qualquer alteração de estado seja identificada
  PORT_B_PULLUPS (0X02);
  kbd_init();  //Inicia biblioteca do teclado matricial

  SSD1306_DrawText(10,30,"INICIANDO",2); //Define posição e texto que será aprensentado no display

  SSD1306_Display(); //Imprime
  delay_ms(3000); //Espera 3 segundos
  
  inicio:  //Ao apertar 'D' essa linha retorna ao inicio do programa
  
  
  while (input(Pin_B6)==1){   // Sempre retorna leitura de do variac para zero por segurança do sistema
         output_high (Pin_A2); 
         output_high (Pin_A0);
         delay_ms(2);
         output_low (Pin_A0);
         delay_ms(2);
  }
  
  output_C (0xff);   //Testa relés do sistema
  output_A (0xff);
  output_E (0xff);
  
  SSD1306_ClearDisplay (); //Limpa display
  SSD1306_DrawText(0,0,"ESCOLHA A GRANDEZA",1);
  SSD1306_DrawText(0,20,"[A] = TENSAO",1);
  SSD1306_DrawText(0,35,"[B] = CORRENTE",1);
  SSD1306_DrawText(0,50,"[C] = RESISTENCIA",1);
  SSD1306_Display();
  
  char c;   //Armazena leitura do teclado
  int quantidade; //Armazena quantidade de equipamentos digitado
  int i = 0;   //Auxilia funções 
  int j = 0;   //Auxilia funções 
  int k = 0;   //Auxilia funções
  int32 aux= 0; // Auxilia funções
  int32 valor = 0;   //Armazena valores de até 32 Bits
  
  
   while (i==0){  //Condição para ler Teclado  
      c = kbd_getc();   //Coleta leitura do teclado e armazena em char 'c'
      delay_ms(1);
      
      switch (c){ //Condição para selecionar opção teclada
         case 'A':   //Caso 'A' tensão 
         
              output_low (Pin_A4);  //Deixa saidas dos relés em estado baixo que faz com que o sistema se torne série
              output_low (Pin_A5);
              output_low (Pin_E0);
              output_low (Pin_E1);
              output_low (Pin_E2);
              output_low (Pin_C2);
              output_low (Pin_C6);
              output_low (Pin_C7);
              
              output_high (Pin_B5); //Ativa Tensão no relé que define Tensão/Resistência
              
              output_low (Pin_C1); //Relé que gera tensão
              
              SSD1306_ClearDisplay ();
              SSD1306_DrawText(0,0,"TENSAO",1.5);
              SSD1306_DrawText(0,20,"[*] = CONTINUA",1);
              SSD1306_DrawText(0,35,"[#] = ALTERNADA",1);
              SSD1306_DrawText(0,50,"[D] = VOLTAR",1);
              SSD1306_Display();
              
              if (c=='D'){
                 goto inicio; //Função para sair se for pressionada a tecla 'D'
              }

              while (i==0){   //Condição para verificar se é continua ou alternada
                 c = kbd_getc();
                 delay_ms (1);
                 
                 if (c=='D'){
                  goto inicio;
                  }
                  
              switch (c){  
                 case '*':
                     Apagar_Continua:  //Retorna caso seja pressionado tecla de Apagar 'A'
                     
                     output_high (Pin_C0); //Relé para definir se irá passar pela ponte retificadora
                     
                     while (i==0){  //Condição que identifica botão referente ao fim de curso do variac na posição inicial
                           
                           SSD1306_ClearDisplay ();
                           SSD1306_DrawText(0,0,"TENSAO CONTINUA",1);
                           SSD1306_DrawText(0,18,"DIGITE:",1);
                           SSD1306_DrawText(63, 16 ,"[#]= MAIS",1);
                           SSD1306_DrawText(63, 26 ,"[*]= MENOS",1);
                           SSD1306_DrawText(63, 36,"[A]= APAGAR",1);
                           SSD1306_DrawText(63, 46,"[C]= ENTER",1);
                           SSD1306_DrawText(63, 56 ,"[D]= VOLTAR",1);
                           SSD1306_Display();
                           
                           while (input(Pin_B6)==1){  //Condição para gerar pulsos para movimento do motor de passo
                              output_high (Pin_A2);   //Define direção do motor de passo
                              output_high (Pin_A0);   //Gera pulso alto
                              delay_ms(2);
                              output_low (Pin_A0);    //Gera pulso baixo
                              delay_ms(2);
                           }
                           
                        while (i==0){
                              c = kbd_getc();
                              delay_ms(1);
                              
                              if (c=='D'){
                                 goto inicio;
                              }
                              
                              if (c=='C'){   //Ao ser pressionada tecla 'C' ele executa os passos do motor
                                 output_low (Pin_A2); //condição que garante sentido do motor
                                 output_low (Pin_B5);   //reles em paralelo
                                float aux = valor *4; //funcao que gera angulo
                                int16 angulo = aux;
                                for (angulo; angulo > 0; angulo --){
                                       output_high (Pin_A0);
                                       delay_ms(3);
                                       output_low (Pin_A0);
                                       delay_ms(3);
                                  }
                              }
                              
                              if (c=='#'){   //Condição para ajuste fino para mais
                                 for (int x = 0; x < 5; x ++){
                                       output_low (Pin_A2);
                                       output_high (Pin_A0);
                                       delay_ms(3);
                                       output_low (Pin_A0);
                                       delay_ms(3);
                                  }
                              }
                              
                              if (c=='*'){   //Condição para ajuste fino para menos
                                 for (int x = 0; x < 5; x ++){
                                       output_high (Pin_A2);   
                                       output_high (Pin_A0);
                                       delay_ms(3);
                                       output_low (Pin_A0);
                                       delay_ms(3);
                                  }
                              }
                              
                              if (c=='A'){   //Condição para retornar e apagar valores já digitados
                                 valor = 0;
                                 j=0;
                                 k=0;
                                 goto Apagar_Continua;
                                 
                              }
                              if (k<3 & (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9')){ //Condição para armazenar, calcular e imprimir teclas digitadas
                                    sprintf(c, "%c", c);
                                    SSD1306_DrawText(40-j*20, 37, c, 3);
                                    SSD1306_Display();
                                    
                                    if (k == 0){
                                       valor = ((int16) c -48);   //Converte caracter em inteiro e armazena na variável valor, sendo o valor de unidade 
                                    }
                                    
                                    if (k == 1){
                                       valor = valor + (((int16) c -48)*10); //Converte caracter em inteiro e armazena na variável valor, sendo multiplicado por 10, portanto é dezena
                                    }
                                    
                                    if (k == 2){
                                       valor =  valor +(((int16) c -48)*100); //Converte caracter em inteiro e armazena na variável valor, sendo multiplicado por 100, portanto é centena
                                    }
                                    j++;
                                    delay_ms(50);
                                    c = 0;
                                    k++;
                              }
                         }
                    }
                
                  case '#':
                     Apagar_Alternada: ////Retorna caso seja pressionado tecla de Apagar 'A'
                     
                     output_low (pin_C0); //Relé para definir se a tensão irá passar pela ponte retificadora 
                     
                     while (i==0){
                        
                        SSD1306_ClearDisplay ();
                        SSD1306_DrawText(0,0,"TENSAO ALTERNADA",1.5);
                        SSD1306_DrawText(0,18,"DIGITE:",1);
                        SSD1306_DrawText(63, 16 ,"[#]= MAIS",1);
                        SSD1306_DrawText(63, 26 ,"[*]= MENOS",1);
                        SSD1306_DrawText(63, 36,"[A]= APAGAR",1);
                        SSD1306_DrawText(63, 46,"[C]= ENTER",1);
                        SSD1306_DrawText(63, 56 ,"[D]= VOLTAR",1);
                        SSD1306_Display();
                        
                        while (input(Pin_B6)==1){
                           output_high (Pin_A2);
                           output_high (Pin_A0);
                           delay_ms(2);
                           output_low (Pin_A0);
                           delay_ms(2);
                        }
                        while (i==0){
                              c = kbd_getc();
                              delay_ms(1);
                              if (c=='D'){
                                 goto inicio;
                              }
                              if (c=='C'){
                                 output_low (Pin_A2);
                                 output_high (Pin_B5);   //reles em paralelo
                                 float aux = valor *4; //funcao que gera angulo
                                 int16 angulo = aux;
                                 for (angulo; angulo > 0; angulo --){
                                       output_high (Pin_A0);
                                       delay_ms(3);
                                       output_low (Pin_A0);
                                       delay_ms(3);
                                  }
                              }
                              if (c=='#'){
                                 for (int x = 0; x < 5; x ++){
                                       output_low (Pin_A2);
                                       output_high (Pin_A0);
                                       delay_ms(3);
                                       output_low (Pin_A0);
                                       delay_ms(3);
                                  }
                              }
                              if (c=='*'){
                                 for (int x = 0; x < 5; x ++){
                                       output_high (Pin_A2);
                                       output_high (Pin_A0);
                                       delay_ms(3);
                                       output_low (Pin_A0);
                                       delay_ms(3);
                                  }
                              }
                              if (c=='A'){
                                 
                                 valor = 0;
                                 j=0;
                                 k=0;
                                 goto Apagar_Alternada;
                                 
                              }
                              if (k<3 & (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9')){ 
                                    sprintf(c, "%c", c);
                                    SSD1306_DrawText(40-j*20, 37, c, 3);
                                    SSD1306_Display();
                                    if (k == 0){
                                       valor = ((int16) c -48);
                                    }
                                    if (k == 1){
                                       valor = valor + (((int16) c -48)*10);
                                    }
                                    if (k == 2){
                                       valor =  valor +(((int16) c -48)*100);
                                    }
                                    j++;
                                    delay_ms(1);
                                    c = 0;
                                    k++;
                             }
                         }
                     }
                 }   
             }
         case 'B': //Opção para definir qual tipo de equipamento será feita a leitura

              int32 valorespiras = 1; //armazena valor de espiras no qual se for escolhido multimetro é inserido 1 para calculo posterior
              
              output_high (Pin_C0); //Relé para coleta de corrente
                    
              SSD1306_ClearDisplay (); //Limpa display
              SSD1306_DrawText(0,0,"TIPO DE EQUIPAMENTO",1);
              SSD1306_DrawText(0,20,"[A] = MULTIMETRO",1);
              SSD1306_DrawText(0,35,"[B] = ALICATE AMP.",1);
              SSD1306_DrawText(0,50,"[D]= VOLTAR",1);
              SSD1306_Display();
              
              
              while (i==0){   //Condição para verificar se é continua ou alternada
                 c = kbd_getc();
                 delay_ms (1);
                 
                 if (c=='D'){
                  goto inicio;
                  }
                 if (c=='A'){
                     goto Apagar_Quantidade;
                 }
                 if (c=='B'){
                    quantidadeespiras:
                    
                    int cont; // 
                    
                    SSD1306_ClearDisplay ();
                    SSD1306_DrawText(0,0,"QUANTIDADE DE ESPIRAS",1.5);
                    SSD1306_DrawText(0,18,"DIGITE:",1);
                    SSD1306_DrawText(63, 16 ,"[A]= APAGAR",1);
                    SSD1306_DrawText(63, 36 ,"[C]= ENTER",1);
                    SSD1306_DrawText(63, 56,"[D]= VOLTAR",1);
                    SSD1306_Display();
                    while (i==0){   //Define quantidade de espira
                       c = kbd_getc();
                       delay_ms (1);
                        if (c=='D'){
                           goto inicio;
                        }
                        if (c=='A'){
                           j=0;
                           k=0;
                           goto quantidadeespiras;
                         }
                        if (c=='C'){
                            goto corrente;
                        }
                        if (k<2 & (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9')){ 
                            sprintf(c, "%c", c);
                            SSD1306_DrawText(28-j*20, 37, c, 3);
                            SSD1306_Display();
                            if (k == 0){
                               valorespiras = ((int16) c -48);
                            }
                            if (k == 1){
                               valorespiras = valorespiras + (((int16) c -48)*10);
                            }
                             j++;
                             delay_ms(1);
                             c = 0;
                             k++;
                         }
                     }
                 }              
              }
              i==0;
              Apagar_Quantidade:
              SSD1306_ClearDisplay ();
              SSD1306_DrawText(0,0,"QUANTIDADE DE MULT.",1.5);
              SSD1306_DrawText(0,18,"DIGITE:",1);
              SSD1306_DrawText(63, 16 ,"[A]= APAGAR",1);
              SSD1306_DrawText(63, 36 ,"[C]= ENTER",1);
              SSD1306_DrawText(63, 56,"[D]= VOLTAR",1);
              SSD1306_Display();
              
               while (i==0){
                   c = kbd_getc();
                   delay_ms(1);
                   if (c=='D'){
                      goto inicio;
                   }
                   if (c=='A'){
                      j=0;
                      k=0;
                      goto Apagar_Quantidade;
                   }
                   if (k>0 & (c=='C')){   //Tecla 'C' confirmar a quantidade e prepara os relés para receber os equipamentos
                   
                          output_high (Pin_A4);  //Deixa saidas dos relés em estado baixo que faz com que o sistema se torne série
                          output_high (Pin_A5);
                          output_high (Pin_E0);
                          output_high (Pin_E1);
                          output_high (Pin_E2);
                          output_high (Pin_C2);
                          output_high (Pin_C6);
                          output_high (Pin_C7);

                         if (quantidade == 8){  //define quantos multimetro serão colocados
                           output_low (pin_C7);
                         }
                         if (quantidade == 7){
                           output_low (pin_C6);
                           output_low (pin_C7);
                         }
                         if (quantidade == 6){
                           output_low (pin_C2);
                           output_low (pin_C6);
                           output_low (pin_C7);
                         }
                         if (quantidade == 5){
                           output_low (pin_E2);
                           output_low (pin_C2);
                           output_low (pin_C6);
                           output_low (pin_C7);
                         }
                         if (quantidade == 4){
                           output_low (pin_E1);
                           output_low (pin_E2);
                           output_low (pin_C2);
                           output_low (pin_C6);
                           output_low (pin_C7);
                         }
                         if (quantidade == 3){
                           output_low (pin_E0);
                           output_low (pin_E1);
                           output_low (pin_E2);
                           output_low (pin_C2);
                           output_low (pin_C6);
                           output_low (pin_C7);
                         }
                         if (quantidade == 2){
                           output_low (pin_A5);
                           output_low (pin_E0);
                           output_low (pin_E1);
                           output_low (pin_E2);
                           output_low (pin_C2);
                           output_low (pin_C6);
                           output_low (pin_C7);
                         }
                         if (quantidade == 1){
                           output_low (pin_A4);
                           output_low (pin_A5);
                           output_low (pin_E0);
                           output_low (pin_E1);
                           output_low (pin_E2);
                           output_low (pin_C2);
                           output_low (pin_C6);
                           output_low (pin_C7);
                         }
                         goto corrente;
               }
               while (k<1 & (c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9')){ //coleta valor de quantidade desejado  
                      sprintf(c, "%c", c);
                      SSD1306_DrawText(20-j*20, 37, c, 3);
                      quantidade = c - '0';
                      SSD1306_Display();
                      j++;
                      k++;
                      delay_ms(50);
                      c = 0;
                      
                   }
               }  

            corrente://Após digitar a quantidade seleciona se o circuito será alternado ou continuo
            SSD1306_ClearDisplay ();
            SSD1306_DrawText(0,0,"CORRENTE",1.5);
            SSD1306_DrawText(0,20,"[*] = CONTINUA",1);
            SSD1306_DrawText(0,35,"[#] = ALTERNADA",1);
            SSD1306_DrawText(0,50,"[D] = VOLTAR",1);
            SSD1306_Display();
            j = 0;
            while (i==0){
              c = kbd_getc();
              delay_ms (1);
              if (c=='D'){
               goto inicio;
              }
              switch (c){
                 case '*':
                     k=0;
                     Apagar_Corrente_Continua:
                     
                     while (i==0){
                        output_high (pin_C0); //Rele da ponte retificadora 
                        SSD1306_ClearDisplay ();
                        SSD1306_DrawText(0,0,"CORRENTE CONTINUA",1);
                        SSD1306_DrawText(0,18,"DIGITE:",1);
                        SSD1306_DrawText(63, 16 ,"[#]= MAIS",1);
                        SSD1306_DrawText(63, 26 ,"[*]= MENOS",1);
                        SSD1306_DrawText(63, 36,"[A]= APAGAR",1);
                        SSD1306_DrawText(63, 46,"[C]= ENTER",1);
                        SSD1306_DrawText(63, 56 ,"[D]= VOLTAR",1);
                        SSD1306_Display();
                        
                        while (input(Pin_B6)==1){
                           output_high (Pin_A2);
                           output_high (Pin_A0);
                           delay_ms(2);
                           output_low (Pin_A0);
                           delay_ms(2);
                        }
                         while (i==0){
                              c = kbd_getc();
                              delay_ms(1);
                              if (c=='D'){
                                 goto inicio;
                              }
                              if (c=='C'){
                                 output_low (Pin_A2);
                                 output_low (Pin_B5);   //reles pra resistor
                                 int16 angulo = ((valor/valorespiras)*86);  //função para quantidade de passos relativos a corrente digitada
                                 for (angulo; angulo > 0; angulo --){
                                       output_high (Pin_A0);
                                       delay_ms(3);
                                       output_low (Pin_A0);
                                       delay_ms(3);
                                  }
                              }
                              if (c=='#'){
                                 for (int x = 0; x < 3; x ++){
                                       output_low (Pin_A2);
                                       output_high (Pin_A0);
                                       delay_ms(3);
                                       output_low (Pin_A0);
                                       delay_ms(3);
                                  }
                              }
                              if (c=='*'){
                                 for (int x = 0; x < 3; x ++){
                                       output_high (Pin_A2);
                                       output_high (Pin_A0);
                                       delay_ms(3);
                                       output_low (Pin_A0);
                                       delay_ms(3);
                                  }
                              }
                              if (c=='A'){
                                 output_high (Pin_B5);   //reles em do resistor
                                 valor = 0;
                                 j=0;
                                 k=0;
                                 goto Apagar_Corrente_Continua;
                                 
                              }
                              if (k<2 & (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9')){
                                    sprintf(c, "%c", c);
                                    SSD1306_DrawText(28-j*20, 37, c, 3);
                                    SSD1306_Display();
                                    if (k == 0){
                                       valor = ((int16) c -48);
                                    }
                                    if (k == 1){
                                       valor = valor + (((int16) c -48)*10);
                                    }
                                    if (k == 2){
                                       valor =  valor +(((int16) c -48)*100);
                                    }
                                    j++;
                                    delay_ms(50);
                                    c = 0;
                                    k++;
                             }
                         } 
                  }
                  case '#':
                     k=0;
                     Apagar_Corrente_Alternada:
                     
                     output_low (pin_C0); //Rele da ponte retificadora
                     
                     while (i==0){

                        SSD1306_ClearDisplay ();
                        SSD1306_DrawText(0,0,"CORRENTE ALTERNADA",1);
                        SSD1306_DrawText(0,18,"DIGITE:",1);
                        SSD1306_DrawText(63, 16 ,"[#]= MAIS",1);
                        SSD1306_DrawText(63, 26 ,"[*]= MENOS",1);
                        SSD1306_DrawText(63, 36,"[A]= APAGAR",1);
                        SSD1306_DrawText(63, 46,"[C]= ENTER",1);
                        SSD1306_DrawText(63, 56 ,"[D]= VOLTAR",1);
                        SSD1306_Display();

                        while (input(Pin_B6)==1){
                           output_high (Pin_A2);
                           output_high (Pin_A0);
                           delay_ms(2);
                           output_low (Pin_A0);
                           delay_ms(2);
                        }
                         while (i==0){
                              c = kbd_getc();
                              delay_ms(1);
                              if (c=='D'){
                                 goto inicio;
                              }
                              if (c=='C'){
                                 output_low (Pin_A2); //identifica fim de curso 
                                 int16 angulo = (valor/valorespiras)*83;
                                 for (angulo; angulo > 0; angulo --){
                                       output_high (Pin_A0);
                                       delay_ms(2);
                                       output_low (Pin_A0);
                                       delay_ms(2);
                                  }
                              }
                              if (c=='#'){
                                 for (int x = 0; x < 2; x ++){
                                       output_low (Pin_A2);
                                       output_high (Pin_A0);
                                       delay_ms(3);
                                       output_low (Pin_A0);
                                       delay_ms(5);
                                  }
                              }
                              if (c=='*'){
                                 for (int x = 0; x < 2; x ++){
                                       output_high (Pin_A2);
                                       output_high (Pin_A0);
                                       delay_ms(3);
                                       output_low (Pin_A0);
                                       delay_ms(3);
                                  }
                              }
                              if (c=='A'){
                                 output_high (Pin_B5);   //reles em do resistor
                                 valor = 0;
                                 j=0;
                                 k=0;
                                 goto Apagar_Corrente_Alternada;
                                 
                              }
                              if (k<2 & (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9')){
                                    sprintf(c, "%c", c);
                                    SSD1306_DrawText(28-j*20, 37, c, 3);
                                    SSD1306_Display();
                                    if (k == 0){
                                       valor = ((int16) c -48);
                                    }
                                    if (k == 1){
                                       valor = valor + (((int16) c -48)*10);
                                    }
                                    if (k == 2){
                                       valor =  valor +(((int16) c -48)*100);
                                    }
                                    j++;
                                    delay_ms(50);
                                    c = 0;
                                    k++;
                             }
                        }  
                    } 
               }   
           }
      
        case 'C':  //Para medir resistencia
          output_low(Pin_B5); //define pino de medição de resistencia
          int addr = 0; // escolhe em qual bit será salvo os valores na memoria ROM
          unsigned int voltas; 
          if (voltas =! 0){
              voltas = read_eeprom (addr); // Lê valores salvo caso não seja zero
          }else{
            voltas = 0;
          }
          
          Apagar_Resistencia:
          voltas = read_eeprom (addr); 
          k=0;
                    
          while (i==0){
             output_low (Pin_B5);   //reles em para resistencia ou variac
             output_low (Pin_A4);
             output_low (Pin_A5);
             output_low (Pin_E0);
             output_low (Pin_E1);
             output_low (Pin_E2);
             output_low (Pin_C2);
             output_low (Pin_C6);
             output_low (Pin_C7);
             SSD1306_ClearDisplay ();
             SSD1306_DrawText(0,0,"RESISTENCIA",1.5);
             SSD1306_DrawText(0,18,"DIGITE:",1);
             SSD1306_DrawText(69, 16 ,"[#]=MAIS",1);
             SSD1306_DrawText(69, 26 ,"[*]=MENOS",1);
             SSD1306_DrawText(69, 36,"[A]=APAGAR",1);
             SSD1306_DrawText(69, 46,"[C]=ENTER",1);
             SSD1306_DrawText(69, 56 ,"[D]=VOLTAR",1);
             SSD1306_Display();

             while (voltas>0){
                output_high (Pin_A3);
                output_high (Pin_A1);
                delay_ms(1);
                output_low (Pin_A1);
                delay_ms(1);
                aux++;
                if (aux >= 250){  //grava valores na memoria ROM após tocar no fim de curso
                    if (input(Pin_B7)==0){ 
                        voltas --;
                        write_eeprom(addr,voltas);
                        delay_ms (2000);
                        aux = 0;
           }
               }
             }
             while (input(Pin_B7)==1){
                output_high (Pin_A3);
                output_high (Pin_A1);
                delay_ms(1);
                output_low (Pin_A1);
                delay_ms(1);
            }
             while (i==0){
                   c = kbd_getc();
                   delay_ms(1);
                   if (c=='D'){
                      goto inicio;
                   }
                   if (c=='C'){
                      output_low (Pin_A3);
                      voltas = 1 ;
                       int32 angulo = valor*6; //funcao para definir passos por angulo
                       for (angulo; angulo > 0; angulo --){
                            output_high (Pin_A1);
                            delay_ms(1);
                            output_low (Pin_A1);
                            delay_ms(1);
                            aux++;
                            if (aux >= 250){
                                 if (input(Pin_B7)==0){
                                    voltas ++;
                                    delay_ms (2000);
                                    aux = 0;
                                    write_eeprom(addr,voltas);
                                 }
                              
                            }
                       }
             
                  }
                   if (c=='#'){
                      for (int x = 0; x < 5; x ++){
                           output_low (Pin_A3);
                           output_high (Pin_A1);
                           delay_ms(2);
                           output_low (Pin_A1);
                           delay_ms(2);
                       }
                  }
                   if (c=='*'){
                       for (int x = 0; x < 5; x ++){
                           output_high (Pin_A3);
                           output_high (Pin_A1);
                           delay_ms(2);
                           output_low (Pin_A1);
                           delay_ms(2);
                        }
                    }
                    if (c=='A'){
                        valor = 0;
                        j=0;
                        k=0;
                        goto Apagar_Resistencia;
                                 
                    }
                    if (k<5 & (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9')){
                        c = c;
                        sprintf(c, "%c", c);
                        SSD1306_DrawText(50-j*13, 37, c, 2);
                        SSD1306_Display();
                    if (k == 0){
                        valor = ((int32) c -48); //valor*1 = unidade
                      }
                      if (k == 1){
                          valor = valor + (((int32) c -48)*10); //valor*10 = dezena
                      }
                      if (k == 2){
                         valor =  valor +(((int32) c -48)*100); //valor*100 = centena
                      }
                      if (k == 3){
                         valor =  valor +(((int32) c -48)*1000);  //valor*1000 = unidade de milhar
                      }
                      if (k == 4){
                         valor =  valor +(((int32) c -48)*10000); //valor*10000 = dezena de milhar 
                      }
                       j++;
                       delay_ms(50);
                       c = 0;
                       k++;
                     }
                 }
             }
         }      
     }
 }