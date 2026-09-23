# Tappa 1 — Il lexer

Prima tappa con del codice. La teoria è chiusa (sessione del 22 set 2026):
qui c'è la **specifica**, non la soluzione. I `.c` e gli `.h` li scrivi tu,
partendo da file vuoti.

---

## 1. Cosa fa, in una frase

Il lexer trasforma un **array di byte** in una **sequenza di token**.

```txt
   "12 + 345"          8 caratteri
        |
        |   LEXER
        v
   NUM(12)  PLUS  NUM(345)  EOF        4 token
```

Due mestieri, e sono solo questi due:

- **scartare** ciò che nessuna regola della grammatica produce (gli spazi);
- **raggruppare** i caratteri che insieme formano un simbolo (le cifre in un
  numero).

## 2. Cosa NON fa

Il confine è caduto tre volte nella sessione di teoria, sempre nello stesso
punto. È il contratto più importante della tappa:

- **il lexer non conosce la grammatica.** Non sa cos'è un'espressione, non sa
  che `*` lega più di `+`. `+` e `*` per lui sono due byte diversi e basta;
- **il lexer non si ricorda cosa è venuto prima.** Non decide se un `-` è
  unario o binario: quella distinzione è posizionale, vive in due righe
  diverse della grammatica, ed è del parser;
- **il lexer non possiede il ciclo.** Non ha un `while` che scandisce tutto il
  file: risponde a una chiamata alla volta. Quindi non può decidere di
  «fermarsi al primo errore» — quella è una politica di chi lo chiama.

Quando ti viene voglia di dare al lexer memoria del contesto, è il parser che
sta bussando alla porta sbagliata.

## 3. Il contratto

> **A ogni chiamata, il lexer restituisce un token. Sempre.**

Non restituisce mai «niente», non restituisce mai un puntatore nullo, non ha
un valore di ritorno che va controllato prima di poter usare il token. Le due
situazioni che verrebbe voglia di trattare come casi speciali sono invece due
tag come tutti gli altri:

- file finito → un token con tag `EOF`. E se lo richiami ancora, `EOF` di
  nuovo, per sempre;
- carattere illegale → un token con tag `ERROR`, e la scansione **prosegue**
  dal carattere dopo.

Il motivo è quello del nodo fittizio nelle liste concatenate, che hai già
scritto ad aprile: **eliminare il caso speciale costa meno che gestirlo bene
in trenta posti diversi.**

## 4. Il token

Tre informazioni, e tutte e tre servono:

| | | |
|---|---|---|
| **tag** | quale dei nove | sempre presente |
| **valore** | il numero, per `NUM` | solo per alcuni tag |
| **posizione** | riga e colonna | sempre presente |

Sul **valore**: `PLUS` non ha valore, `NUM` sì. Quindi il tag non è
un'etichetta descrittiva, è il **permesso di leggere l'altro campo**. È la
tagged union della Tappa 0, che arriva per la prima volta su qualcosa che
compili davvero.

Sulla **posizione**: sta in *ogni* token, non solo in quello di errore. Alla
Tappa 2 il parser dovrà dire *«hai aperto questa parentesi e non l'hai
chiusa»* puntando a un `(` incontrato venti token prima. O quella posizione se
l'è portata dietro il token, o è persa.

### I nove tag

```txt
   NUM       una sequenza di cifre            porta un valore
   PLUS      +
   MINUS     -        (uno solo: non esiste un tag "meno unario")
   STAR      *
   SLASH     /
   LPAREN    (
   RPAREN    )
   EOF       input finito                     sentinella
   ERROR     carattere illegale               porta il carattere colpevole
```

## 5. Specifica lessicale

**Spazi.** Si scartano: spazio, tab, `\r`, `\n`. Non producono nessun token.

**Numeri.** Una o più cifre `0`–`9`. Niente segno (il `-` è un token a sé),
niente punto decimale, niente notazione esponenziale. Il valore è un intero
con segno a 64 bit (`long`). I numeri in virgola mobile arriveranno, ma non
qui: aggiungerebbero il confine int/float a una tappa che parla d'altro.

La regola di raggruppamento è quella che hai trovato tu, ed è **positiva**:

> continuo a mangiare caratteri **finché il carattere in mano è una cifra**.

Non un elenco di terminatori. Un elenco obbliga il codice che legge i numeri a
conoscere tutto il resto del linguaggio, si allunga a ogni operatore nuovo, e
quando te ne dimentichi uno fallisce **in silenzio**: su `12%3` produce un
`NUM` con dentro spazzatura invece di un errore. La condizione positiva guarda
solo il byte che ha in mano, e le cifre sono dieci per sempre. Nome proprio:
**maximal munch**.

Conseguenza gratis: l'EOF smette di essere un caso a parte, perché non è una
cifra.

**Caratteri illegali.** Qualunque byte che non sia uno spazio, una cifra o uno
dei sei operatori. Produce un token `ERROR` che porta il byte colpevole, e la
scansione riprende **dal carattere successivo**. Un file con tre caratteri
illegali produce tre token `ERROR`, come `gcc` ti mostra tre errori e non uno.

## 6. Riga e colonna

Il lexer ha in mano un puntatore dentro un buffer. Da lì, gratis, ottiene una
cosa sola: l'**offset**, `p - inizio`, perché *è* il puntatore.

Riga e colonna **non si ricavano da un puntatore**. Si mantengono durante la
scansione:

```txt
   buffer:   1 + 2 \n 3 * @ \n
                          ^
                          p

   p - inizio  =  8        <- gratis, ed è un offset, non una colonna
   riga        =  2        <- perché hai contato un '\n'
   colonna     =  5        <- perché l'hai azzerata su quel '\n'
```

Regola: riga parte da **1** e cresce di 1 a ogni `\n`; colonna parte da **1**,
si azzera (torna a 1) subito dopo un `\n`, e cresce di 1 per ogni altro byte
consumato. Un tab conta **una** colonna.

La posizione registrata in un token è quella del suo **primo** carattere.

Il **nome del file** non è deducibile da niente: non sta nel buffer, non sta
nel puntatore. Arriva dall'esterno, e se il lexer lo deve stampare se lo deve
tenere.

## 7. Chi possiede la memoria

Contratto stretto, e vale la pena rispettarlo alla lettera perché alla Tappa 2
cambia e vorrai vedere cosa cambia:

- `main` apre il file, lo legge **tutto** in un buffer che alloca lui,
  terminato da `\0`, e alla fine lo libera lui;
- il **lexer non alloca niente e non libera niente.** Riceve in prestito il
  puntatore al buffer e ci si muove sopra;
- il token è un valore piccolo, si copia. Nessun token contiene puntatori a
  memoria allocata (il valore di `NUM` è un `long`, non una stringa).

Quindi in tutta la tappa c'è **una sola** `malloc` e **una sola** `free`, e
stanno tutte e due in `main`.

## 8. Il programma da consegnare

Un eseguibile `mango` che prende **un argomento**, il nome di un file, lo
scandisce e stampa i token su `stdout`, uno per riga, fino a `EOF` compreso.

```
$ ./mango prova.mango
```

### Formato di output

Una riga per token. Il formato è **esatto**: i test lo controllano carattere
per carattere.

```txt
   TAG riga:colonna                  per PLUS MINUS STAR SLASH LPAREN RPAREN EOF
   NUM valore riga:colonna           per NUM
   ERROR 'c' riga:colonna            per ERROR, con il byte colpevole fra apici
```

Esempio completo. Input:

```txt
12 + (345 * 6)
```

Output atteso:

```txt
NUM 12 1:1
PLUS 1:4
LPAREN 1:6
NUM 345 1:7
STAR 1:11
NUM 6 1:13
RPAREN 1:14
EOF 1:15
```

Il valore di `NUM` è stampato in decimale **come numero**, non come i
caratteri letti: `007` stampa `NUM 7`.

### Codici di uscita

| | |
|---|---|
| `0` | scansione completata, nessun token `ERROR` prodotto |
| `1` | scansione completata, **almeno un** token `ERROR` prodotto |
| `2` | il file non si apre, o manca l'argomento. Messaggio su `stderr` |

Nota bene dove sta questa decisione: il lexer continua a produrre token dopo
un errore, e stampa tutto fino a `EOF`. È `main` che, alla fine, si ricorda se
ne ha visto almeno uno e sceglie il codice di uscita. **Politica del
chiamante**, esattamente come avevi detto tu.

## 9. Le micro-tappe

Un pezzo per sessione. `make` pulito e i test di quel pezzo verdi prima di
passare al successivo.

- [ ] **1 — lo scheletro.** `main` legge il file in un buffer, stampa quanti
      byte ha letto, libera. Nessun token. Gestisce: argomento mancante, file
      inesistente, file vuoto. *Verifica: `make` senza warning e ASan pulito.*
- [ ] **2 — i token a un carattere.** I sei operatori più `EOF`, con lo scarto
      degli spazi e la contabilità di riga e colonna. Ancora nessun numero.
- [ ] **3 — i numeri.** Maximal munch e conversione a `long`.
- [ ] **4 — il carattere illegale.** Il tag `ERROR` e i codici di uscita.
- [ ] **5 — tutti i test verdi**, `-Wconversion` compreso, ASan e UBSan
      compresi.
- [ ] **6 — extra: l'overflow.** Un numero che non entra in un `long` non è un
      `NUM`. Vedi in fondo.

## 10. Invarianti da tenere d'occhio

Sono le cose che i test non controllano ma che una rilettura sì:

1. **Ogni percorso nel codice consuma almeno un byte, oppure restituisce
   `EOF`.** Se esiste un percorso che non fa né l'uno né l'altro, hai un ciclo
   infinito su un input che non hai ancora provato.
2. **Nessuna lettura oltre il `\0` finale.** Il classico è guardare il
   carattere *dopo* quello in mano senza prima aver controllato quello in
   mano.
3. **La posizione salvata in un token è quella del suo primo carattere**, non
   quella raggiunta dopo averlo scandito. Su `NUM 345` è la colonna del `3`.
4. **Il lexer non chiama `printf`.** La stampa è di `main`. Se il lexer
   stampa, alla Tappa 2 non lo puoi riusare.

## 11. Extra — l'overflow

Da fare solo dopo che i test principali sono verdi.

`99999999999999999999` è una sequenza legale di cifre, e non entra in un
`long`. Cosa succede oggi nel tuo codice? Dipende da come converti, e in
almeno un modo comune il comportamento è **undefined**, non "wrap".

Quando lo affronti, il token diventa un `ERROR` con un dettaglio diverso dal
carattere:

```txt
   ERROR overflow riga:colonna
```

I test dell'overflow sono nel file di test, marcati `[extra]`.

---

## 12. Confronto col sorgente vero

*Da scrivere da te, alla fine della tappa, dopo aver letto `scanner.c` di clox
(clonato in `/tmp`, non qui). Domanda guida: **dove ha scelto diversamente, e
cosa sapeva lui che io non sapevo?***
