# Mango Grammar

La grammatica del nucleo aritmetico di mango. È il contratto delle Tappe 1
(lexer) e 2 (parser): quando lì qualcosa non torna, la risposta è qui.

```txt
expr    ->  term | expr '+' term | expr '-' term
term    ->  unary | term '*' unary | term '/' unary
unary   ->  '-' unary | factor
factor  ->  NUM | '(' expr ')'
```

Simbolo iniziale: `expr`.
Terminali: `NUM`, `'+'`, `'-'`, `'*'`, `'/'`, `'('`, `')'`.

Quattro livelli, quattro forze di legame. **La precedenza è la profondità
nelle regole**: `*` lega più forte di `+` perché `term` sta sotto `expr`, non
perché esista una tabella di priorità da qualche parte.
**L'associatività è la posizione del non terminale ricorsivo** dentro la
regola.

---

## 1. I livelli, annotati

### `expr` — il livello più debole

```txt
expr -> term | expr '+' term | expr '-' term
```

**Operatori:** `+` e `-`.

**Perché stanno insieme, e non su due livelli.** `1 - 2 * 3` fa **-5**, non
-3. Se `-` e `*` fossero alla stessa forza si raggrupperebbe `(1-2)*3 = -3`;
il valore atteso è -5, quindi `*` lega più forte di `-`, esattamente come lega
più forte di `+`. Due operatori di pari forza non chiedono un livello nuovo:
chiedono **un'alternativa in più sullo stesso livello**.

**Associatività: a sinistra.** `10 - 4 - 3`:

```txt
    raggruppa a SINISTRA          raggruppa a DESTRA
    (10 - 4) - 3  =  3            10 - (4 - 3)  =  9

           (-)                          (-)
          /   \                        /   \
        (-)   (3)                   (10)   (-)
       /   \                              /   \
    (10)   (4)                          (4)   (3)
```

**3** contro **9**. Chi scrive `10 - 4 - 3` intende 3, quindi sinistra. La
stringa dimostra perché ha due valori diversi: `10 + 4 + 3` non dimostrerebbe
niente, `+` è associativo sugli interi e i due alberi collassano su 17.

**Come lo impone la regola.** Il non terminale ricorsivo sta **a sinistra**
del terminale: `expr '+' term`. Il lato destro di ogni `+` è un `term`, che
non può essere una catena di `+`; la catena può accumularsi solo a sinistra.
Con `term '+' expr` si otterrebbe l'albero di destra e il valore 9.

**Posizione nella gerarchia.** È il livello più alto, quindi la forza di
legame più debole, quindi il simbolo iniziale: un programma di mango *è*
un'`expr`. Tutto ciò che lega più forte gli sta sotto e si riduce prima.

**Costo alla Tappa 2.** Ricorsiva a sinistra → `expr()` chiamerebbe se stessa
senza aver consumato token: loop infinito. Si scriverà come iterazione.

### `term`

```txt
term -> unary | term '*' unary | term '/' unary
```

**Operatori:** `*` e `/`.

**Perché stanno insieme, e sotto `expr`.** `8 / 4 + 1` fa **3**, non 1.6. Se
`/` vivesse al livello di `+` si raggrupperebbe `8 / (4 + 1) = 1.6`; il valore
atteso è 3, quindi `/` lega più forte di `+`, cioè sta dove sta già `*`.
Stesso ragionamento di `expr`, altra coppia: un'alternativa in più, non un
livello in più.

**Associatività: a sinistra.** `8 / 4 / 2`:

```txt
    raggruppa a SINISTRA          raggruppa a DESTRA
    (8 / 4) / 2  =  1             8 / (4 / 2)  =  4

           (/)                          (/)
          /   \                        /   \
        (/)   (2)                    (8)   (/)
       /   \                              /   \
     (8)   (4)                          (4)   (2)
```

**1** contro **4**. Chi scrive `8 / 4 / 2` intende 1, quindi sinistra. Come
sopra, la stringa deve usare `/` e non `*`: `8 * 4 * 2` fa 64 in entrambi i
raggruppamenti e non dimostra niente.

**Come lo impone la regola.** Non terminale ricorsivo a sinistra:
`term '*' unary`. Il lato destro di ogni `*` è un `unary`, mai una catena.

**Costo alla Tappa 2.** Identico a `expr`: ricorsiva a sinistra → iterazione.
`expr` e `term` sono gemelli, e le due funzioni avranno la stessa forma con
due insiemi di operatori diversi.

### `unary`

```txt
unary -> '-' unary | factor
```

**Operatore:** `-` prefisso. Un solo operando, non due.

**Perché non in cima a `expr`.** `-2 + 3` fa **1**, non -5. Un prefisso messo
sopra `expr` avrebbe la forza di legame più debole di tutte e si mangerebbe
l'addizione alla sua destra: `-(2 + 3) = -5`. Il valore atteso è 1, quindi il
meno unario lega **più forte** di `+`, e va sotto `expr`.

**Qui non c'è un'associatività da scegliere.** `- -5` ha un solo
raggruppamento possibile — `-(-5)`. Non esistono due alberi da confrontare,
quindi la domanda dell'associatività, che è *"dei due raggruppamenti, quale?"*,
non si pone nemmeno: un operatore prefisso non ha niente alla sua sinistra da
raggruppare. La direzione della ricorsione non è una scelta, è imposta dalla
forma dell'operatore.

**Cosa dimostra allora `- -5`.** Che il livello **si richiama**, cioè che
l'operando del meno unario è un altro `unary` e non un `factor`. Con la regola
mutilata

```txt
unary -> '-' factor | factor
```

`- -5` non sarebbe derivabile affatto: consumato il `-`, servirebbe un
`factor`, e `-5` non è un `factor`. La stringa non sceglie fra due alberi —
sceglie fra **un albero e nessun albero**. È un tipo di dimostrazione diverso
da quello di `expr` e `term`, ed è il motivo per cui questa sezione non ha la
stessa forma delle altre due.

**Costo alla Tappa 2.** È l'unica regola ricorsiva **a destra**, e l'unica che
si scrive come ricorsione vera. Funziona perché il `-` viene **consumato prima**
della chiamata: l'input residuo si accorcia strettamente a ogni giro, quindi la
discesa ha un fondo garantito (al massimo tanti livelli quanti sono i token).
La ricorsione a sinistra non consuma niente prima di richiamarsi, e per questo
non ha fondo.

```txt
SINISTRA  expr -> expr '+' term     DESTRA  unary -> '-' unary

expr()  input: 1+2  -> chiama       unary()  input: - - 5  consuma, chiama
expr()  input: 1+2  -> chiama       unary()  input: - 5    consuma, chiama
expr()  input: 1+2  -> chiama       unary()  input: 5      -> factor()
  input identico: stack overflow      input più corto: termina
```

### `factor`

```txt
factor -> NUM | '(' expr ')'
```

**Nessun operatore.** Questo livello non risponde alla domanda *"chi lega più
forte"*, perché non c'è niente da legare. Risponde a un'altra domanda:
**cosa può stare al posto di un operando, qualunque sia l'operatore sopra.**
È il fondo della gerarchia, e tutte le catene degli altri livelli finiscono
qui.

Due alternative, e fanno due mestieri opposti.

**`NUM` — dove la derivazione tocca terra.** L'unico terminale che porta un
valore. È il caso base: senza di lui nessuna derivazione terminerebbe.

**`'(' expr ')'` — il ciclo.** La gerarchia non è lineare, è un anello:

```txt
   expr  --nomina-->  term  --nomina-->  unary  --nomina-->  factor
     ^                                                          |
     +--------------------- '(' expr ')' ----------------------+
```

Dentro una parentesi si riparte dal **simbolo iniziale**, quindi lì dentro si
può scrivere di nuovo qualunque cosa, `+` compresi. È l'unico modo di far
scendere un operatore debole sotto uno forte, ed è la controparte grammaticale
della frase del README: *le parentesi non aggiungono un livello, scambiano chi
sta sopra*.

Le parentesi **non lasciano traccia nell'albero**: sono consumate dalla regola
di `factor`, e il nodo prodotto è quello che stava dentro. Non sono
un'operazione, sono un'istruzione al parser.

**Costo alla Tappa 2.** Unica regola **non ricorsiva**. Non cicla e non si
richiama: guarda il prossimo token e smista. Si scrive come `switch` / `if`.

### Le quattro forme, in tabella

| Livello | Forma della ricorsione | Forma della funzione |
|---|---|---|
| `expr` | a sinistra | ciclo |
| `term` | a sinistra | ciclo |
| `unary` | a destra | autochiamata, dopo aver consumato il prefisso |
| `factor` | nessuna | dispatch sul prossimo token |

Quattro righe, tre forme diverse. Non è un dettaglio estetico: è il criterio
con cui è stata presa la decisione 2 qui sotto.

---

## 2. Le tre decisioni

Sono di due nature diverse, e confonderle è il modo più rapido di riaprire la
discussione fra sei settimane.

- La decisione **3** cambia il linguaggio: con l'alternativa, certe stringhe
  smettono di essere derivabili.
- Le decisioni **1** e **2** non cambiano niente di osservabile — stesse
  stringhe accettate, stessi alberi prodotti. Sono decisioni sul **parser** e
  sulla leggibilità del contratto, e vanno difese su quel terreno, non
  fingendo che il linguaggio cambi.

### 1. Niente `addop -> '+' | '-'`

Fattorizzare gli operatori in un non terminale è tentante:

```txt
expr  -> term | expr addop term
addop -> '+' | '-'
```

Scartata. `addop` sarebbe l'unico non terminale che **non corrisponde a un
nodo dell'albero**.

Nell'AST di `1 + 2` ci sono tre nodi, e nessuno dei tre è il `+`:

```txt
   (+)        <- nodo binario: il '+' è il suo TAG, non un figlio
  /   \
(1)   (2)
```

L'operatore non è un nodo, è il **tag** del nodo binario — un campo della
tagged union, esattamente come il discriminante misurato nella sessione 1.
Un non terminale che corrisponde a un *campo* invece che a un *nodo* rompe la
corrispondenza fra grammatica e albero, e quella corrispondenza è l'unica
bussola disponibile alla Tappa 2.

Cosa si rompe in concreto: ogni altra funzione del parser restituirà un
`Node *`. `addop()` restituirebbe un tipo di token. Una funzione con un tipo
di ritorno diverso da tutte le altre, che non alloca niente e non costruisce
niente, in mezzo a quattro che fanno il contrario.

Il guadagno sarebbe una riga di grammatica più corta. Non basta.

> **Nota su un argomento sbagliato usato il 18/09/2026:** era stato detto che
> `addop` riduce il costo perché il parser prova "2 alternative invece di 3".
> È un claim di performance senza misura, e per giunta su un modello di parser
> con backtracking che non è quello che si scriverà. Un parser a discesa
> ricorsiva non "prova" le alternative: guarda un token e salta. La decisione
> è giusta, quella motivazione no.

### 2. `unary` come livello a sé, non dentro `factor`

Le due candidate:

```txt
B) scelta                            C) scartata

unary  -> '-' unary | factor         factor -> '-' factor
factor -> NUM | '(' expr ')'                 | NUM | '(' expr ')'
```

**B e C generano lo stesso linguaggio e gli stessi alberi.** Non è
un'impressione: è dimostrato, e senza cercare controesempi.

Due posizioni diverse nella gerarchia producono alberi diversi **solo se fra
loro vive un operatore che possa restare intrappolato** da una e non
dall'altra. Fra `factor` e `unary` non c'è nessun livello, quindi nessun
operatore, quindi nessuna stringa può distinguerle. Il controesempio non è
*difficile da trovare*: non **può esistere**.

(È un argomento più forte del controesempio. Un controesempio dimostra che due
cose differiscono; questo dimostra che non possono differire.)

Quindi la scelta non si decide sugli alberi. Si decide a valle, sulla tabella
delle quattro forme:

- con **B**, ogni funzione del parser ha **una forma sola**: `factor()`
  smista, `unary()` si richiama;
- con **C**, `factor()` dovrebbe fare **due cose strutturalmente diverse nello
  stesso corpo** — il dispatch sul token *e* l'autochiamata ricorsiva a destra,
  intrecciate.

Una riga di grammatica in più, una funzione più semplice. Il contratto è
scritto una volta, il parser si legge per dieci tappe.

### 3. `factor -> '(' expr ')'`, non `'(' term ')'`

Questa cambia il linguaggio, e lo mutila.

Il non terminale scritto fra le parentesi decide **quanto in alto si risale**
quando ci si entra. Con `'(' term ')'` si risale solo fino a `term`, cioè al
livello di `*` e `/`: dentro una parentesi si potrebbero scrivere solo catene
di moltiplicazioni.

`(1 + 2)` diventa **non derivabile**. `(4 * 5)` si deriva ancora, e non serve a
niente: sono già le stesse regole che valgono fuori.

E si perde l'unica ragione per cui le parentesi esistono. Il loro mestiere è
far scendere un operatore *debole* sotto uno *forte* — l'unica cosa che la
gerarchia dei livelli, da sola, rende impossibile. Puntare a `term` toglie
esattamente quella capacità e lascia solo la punteggiatura.

Regola generale: **le parentesi devono puntare al simbolo iniziale**, o sono
decorative.

---

## 3. Due derivazioni

Derivazioni **canoniche sinistre**: a ogni passo si espande il non terminale
più a sinistra. Una sola sostituzione per passo — ogni passo è un nodo
dell'albero di derivazione, e due sostituzioni insieme farebbero smettere la
derivazione di *essere* l'albero.

L'ordine di espansione non cambia l'albero (è una proprietà del procedimento,
non della grammatica), ma la sinistra è quella che un parser a discesa
ricorsiva costruisce davvero, leggendo i token da sinistra a destra. Da lì il
nome della famiglia: **LL**, *Left-to-right input, Leftmost derivation*.

### 3.1 Con le parentesi — `(1 + 2) * 3`

```txt
 1.  expr
 2.  term                               expr   -> term
 3.  term '*' unary                     term   -> term '*' unary
 4.  unary '*' unary                    term   -> unary
 5.  factor '*' unary                   unary  -> factor
 6.  '(' expr ')' '*' unary             factor -> '(' expr ')'
 7.  '(' expr '+' term ')' '*' unary    expr   -> expr '+' term
 8.  '(' term '+' term ')' '*' unary    expr   -> term
 9.  '(' unary '+' term ')' '*' unary   term   -> unary
10.  '(' factor '+' term ')' '*' unary  unary  -> factor
11.  '(' NUM '+' term ')' '*' unary     factor -> NUM
12.  '(' NUM '+' unary ')' '*' unary    term   -> unary
13.  '(' NUM '+' factor ')' '*' unary   unary  -> factor
14.  '(' NUM '+' NUM ')' '*' unary      factor -> NUM
15.  '(' NUM '+' NUM ')' '*' factor     unary  -> factor
16.  '(' NUM '+' NUM ')' '*' NUM        factor -> NUM

      (    1   +   2    )   *   3
```

Albero di derivazione — un nodo per ogni passo, i figli sono la parte destra
della regola applicata:

```txt
                        expr
                         |
                        term
                    /    |    \
                term    '*'    unary
                 |               |
               unary           factor
                 |               |
               factor          NUM(3)
             /   |   \
           '('  expr  ')'
              /   |   \
          expr   '+'   term
           |             |
          term         unary
           |             |
         unary         factor
           |             |
         factor        NUM(2)
           |
         NUM(1)
```

AST — resta solo ciò che serve a calcolare:

```txt
        (*)
       /   \
     (+)   (3)
    /   \
  (1)   (2)
```

Da **21 nodi a 5**. Spariscono tutti gli `expr`, `term`, `unary`, `factor`:
sono impalcatura, esistono per *imporre* la precedenza durante il parsing, e
una volta imposta la precedenza è nella forma dell'albero e non serve più.
Spariscono anche le due parentesi: hanno fatto il loro lavoro al passo 6,
mettendo `expr` sotto un `*`, e non sono un'operazione da eseguire.

### 3.2 Col meno unario — `-2 + 3`

```txt
 1.  expr
 2.  expr '+' term            expr   -> expr '+' term
 3.  term '+' term            expr   -> term
 4.  unary '+' term           term   -> unary
 5.  '-' unary '+' term       unary  -> '-' unary
 6.  '-' factor '+' term      unary  -> factor
 7.  '-' NUM '+' term         factor -> NUM
 8.  '-' NUM '+' unary        term   -> unary
 9.  '-' NUM '+' factor       unary  -> factor
10.  '-' NUM '+' NUM          factor -> NUM

      -   2   +   3
```

Albero di derivazione:

```txt
                  expr
              /    |    \
          expr    '+'    term
           |               |
          term           unary
           |               |
         unary           factor
        /     \             |
      '-'    unary        NUM(3)
               |
             factor
               |
             NUM(2)
```

AST:

```txt
        (+)
       /   \
   (neg)   (3)
     |
    (2)
```

Due cose da leggere in questo albero:

- il nodo unario ha **un figlio solo**. È la ragione per cui il nodo dell'AST
  non può essere una struct a due puntatori, e da lì tutta la storia della
  tagged union;
- il `'-'` del passo 5 e il `'+'` del passo 2 **non sono nodi**: sono i tag
  del nodo unario e del nodo binario. Nell'AST ci sono 4 nodi e nessun
  operatore fra questi. È la decisione 1 vista dal lato dell'albero.

Valore: **1**. Con il meno unario in cima a `expr` sarebbe stato `-(2+3)`,
cioè -5, e l'albero avrebbe avuto `(neg)` alla radice con `(+)` sotto.

---

## 4. Nota sul nome del file

Il file è `grammar.md`, e il `README.md` della tappa lo cita già così
(righe 507 e 533). Nessun disallineamento da correggere: la nota in
`LEARNING.md` che parlava di un `grammatica.md` era vecchia.
