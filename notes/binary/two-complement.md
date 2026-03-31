Rappressenta un passaggio in più rispetto al complemento a 1:
in un sistema a 4 bit, anzichè fermarsi a 1111, si punta a raggiungere 10000
che si ottiene sommando 1 a 1111 (trattasi di **overflow**), il risultato diventa 0000!

In questo modo si ottiene l'eliminazione della doppia rappresentazione dello 0, che era il problema principale del complemento a 1

Sommando l'uno finale, spingo il numero oltre il limite dei bit disponibili, facendolo tornare a 0

In questo modo si capisce perchè un valore cone 11111011 può avere due significati:

se il programma lo legge come _unsigned_, il valore è **251**
se lo legge come _signed_, il suo valore è -5 perchè è la distanza che manca per tornare a 0 -> 251 + 5 === 256 (1 valore di overflow rispetto a 255 e si torna a 0)

rappresentare un numero binario insieme al suo segno.
per fare spazio ai numeri negativi, quelli positivi sono dimezzati.

8 bit = 2⁸ = 256 caratteri;
8bit unsigned = 0 -> +255
8bit A2 = -128 -> +127 (comporta che se sono a 127 e aggiungo 1 vado a -128)

il concetto è sfruttare l'overflow per sottrarre -128 ai numeri?

BIT NUMERI POSITIVI/NEGATIVI                            N
-128    64    32    16    8    4    2    1
   0     0     0     0    0    0    0    1              1 (decimale)

   1     1     1     1    1    1    1    1              -1 (decimale)
   (1 + 2 + 4 + 8 + 16 + 32 + 64) - 128 = -1

il ragionamento è: se un bit è posto a 1, aggiungo la cifra corrispondente, altrimenti non faccio nulla;

MSB -> most significant bit
il bit più significativo è negativo e ha un valore matematico negativo

complemento a 1: invertire i bit (0 -> 1, 1 -> 0)
0011 (+2+1=+3)
1100 (-8+4=-4)

il problema del complemento a 1 è che lascia con due rappresentazioni dello 0
0 positivo -> 0000
0 negativo -> 1111 (-8-6-4-2-1)

aggiungendo 1 rimuoviamo la doppia rappresentazione dello 0


