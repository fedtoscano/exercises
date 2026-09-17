# Tappa 0 — Cos'è un linguaggio

Il linguaggio si chiama **mango**. Deciso il 2026-09-17, prima decisione di
design del progetto.

Nessun codice in questa tappa. Carta, penna, e due misure fatte col
compilatore. Serve a costruire il modello mentale che regge le dieci tappe
successive.

---

## 1. Un linguaggio è una catena di rappresentazioni

Non una cosa sola: una sequenza di forme diverse dello stesso programma, dove
ogni passaggio butta via qualcosa di irrilevante e rende possibile il
passaggio dopo.

```
  "1 + 2 * 3"                     testo: un array di char, niente di più
       |
       |  LEXER  — raggruppa i caratteri in parole, butta gli spazi
       v
  [NUM 1] [PLUS] [NUM 2] [STAR] [NUM 3]        token: una LISTA, lineare
       |
       |  PARSER — scopre la struttura nascosta nella lista
       v
            (+)                                albero: ha una FORMA
           /   \
        (1)     (*)
               /   \
            (2)     (3)
       |
       |  INTERPRETE — visita l'albero e lo riduce
       v
            7                                  valore
```

Il salto che conta è dalla lista all'albero.

La lista di token è **lineare**: sa dire solo "questo viene prima di quello".
Non c'è spazio, in una lista, per scrivere che `*` lega più forte di `+`.

L'albero ha una forma, e **la forma è la precedenza**: `2 * 3` sta più in
basso, quindi si riduce prima. L'interprete non applica nessuna regola di
precedenza — visita l'albero dal basso, stupidamente. La precedenza è stata
decisa una volta sola, dal parser, e congelata nella struttura.

L'albero non è una comodità implementativa: è l'unica forma che può contenere
quell'informazione.

## 2. Le parentesi non esistono nell'albero

```
   1 + 2 * 3              (1 + 2) * 3
       (+)                    (*)
      /   \                  /   \
   (1)     (*)            (+)     (3)
          /   \          /   \
       (2)     (3)    (1)     (2)
```

Le parentesi non sono un'operazione: sono **un'istruzione al parser**. Vivono
solo durante il parsing e non lasciano traccia. Non aggiungono un livello —
scambiano chi sta sopra.

Verifica del concetto: se dall'albero volessi ristampare il sorgente, le
parentesi le dovresti **ridedurre dalla forma** (ne serve una attorno a un
figlio quando il suo operatore lega più debolmente del padre), perché non sono
salvate da nessuna parte.

## 3. Associatività

`1 + 2 + 3` è ambiguo se guardi solo i token: `(1+2)+3` o `1+(2+3)`? Due alberi
diversi, e uno va scelto.

Per `+` e `*` il risultato non cambia — sono associativi sugli interi. Per `-`
e `/` cambia eccome:

```
8 / 4 / 2   ->  (8/4)/2 = 1      1 - 2 - 3  ->  (1-2)-3 = -4
            ->  8/(4/2) = 4                 ->  1-(2-3) =  2
```

Il C, Python e chiunque legga da sinistra rispondono **1** e **-4**: si
raggruppa da sinistra. È **associatività a sinistra**, ed è la convenzione di
`+ - * /`. Non è arbitraria: te la impone la sottrazione, perché `8/4/2` deve
fare quello che si aspetta chi lo scrive.

Associano a destra, e sono pochi e riconoscibili:

```
a = b = c     ->  a = (b = c)
2 ^ 3 ^ 2     ->  2 ^ (3 ^ 2) = 512,  non (2^3)^2 = 64
```

**Nota da riprendere:** "`+` e `*` sono associativi" è vero per gli **interi** e
falso per i `double`. Con i float `(a+b)+c != a+(b+c)`, e in un accumulo di
campioni audio la differenza si vede.

## 4. Il tipo di un nodo — il problema vero della tappa

Contando i figli dei nodi reali:

| Nodo | Figli |
|---|---|
| `1` (letterale) | 0 — nessuna sotto-espressione, ma **un valore** |
| `-x` (meno unario) | 1 |
| `a + b` | 2 |
| `if / else` | 2 o 3 |
| `f(a, b, c)` | **N**, e N si sa solo a runtime |

Il binario a due figli è un caso, non la regola. Quindi un nodo non ha una
forma sola: il letterale porta un numero e nessun figlio, il binario due
puntatori e nessun numero, la chiamata un array di figli di lunghezza
variabile. **Tre layout di memoria diversi, che devono stare nello stesso
tipo**, perché il figlio sinistro di un `+` può essere uno qualsiasi dei tre.

### 4.1 Perché serve un tag

Prima idea: una struct sola con tutti i campi possibili.

```
offset  0        8        16       24       32    36   40
        +--------+--------+--------+--------+-----+----+
        | value  | left   | right  | args   |argc |....|
        +--------+--------+--------+--------+-----+----+
         \______/ \_______________/ \____________/  ^
         letterale   binario          chiamata      padding
```

`sizeof` = **40**. Il letterale `1` ne usa 8 su 40. L'albero di `1 + 2 * 3` ha
5 nodi, 3 dei quali letterali: 200 byte allocati per trasportarne 56.

E c'è un problema peggiore dello spreco. Data una funzione che riceve un
nodo, come fa a sapere se `left` è popolato? Legge `left` — e dentro ci sono
**sempre** 8 byte, che valgono un numero qualsiasi. In C non esiste nessun bit
accanto al campo che dica "questo non l'ha scritto nessuno". Se il nodo arriva
da `malloc`, `left` contiene immondizia indistinguibile da un puntatore
valido. E azzerando tutto non si risolve: il nodo del letterale `0` e il nodo
del `+` hanno `value` identico, bit per bit, e significato opposto.

> L'informazione "quali campi di questo oggetto sono validi" **non è dentro
> l'oggetto**. Non è deducibile. Va messa lì esplicitamente, come dato.

Quel campo è il **tag**, o discriminante. Non lo leggi per calcolare: lo leggi
per sapere cosa hai il diritto di leggere dopo.

(In Python un oggetto sa di che tipo è perché in CPython ogni oggetto comincia
con un campo `ob_type`. Cioè perché qualcuno, in C, ha scritto il tag a mano.)

### 4.2 `union`

I tre gruppi sono **disgiunti nel tempo**: un nodo è un letterale, *oppure* un
binario, *oppure* una chiamata — mai due insieme, in nessun momento della sua
vita. Eppure nella struct grassa occupano indirizzi diversi e li paghi tutti.

Una `union` è come una struct con una sola differenza: **tutti i membri
partono dall'offset 0**, sovrapposti sugli stessi byte.

```
struct                              union
+--------+--------+--------+        +--------+
| a (8)  | b (8)  | c (4)  |        |   a    |  8
+--------+--------+--------+        |  b     |  8   tutti da offset 0
 sizeof = somma (+ padding)         | c  |       4
                                    +--------+
                                     sizeof = il più grande
```

Due regole:

1. `sizeof` di una union è quella del membro **più grande**, arrotondata per
   rispettare l'allineamento del membro più esigente.
2. Il C **non tiene traccia** di quale membro hai scritto per ultimo. Se
   scrivi `value` e poi leggi `left`, ottieni i bit del `double`
   reinterpretati come indirizzo, senza un warning.

La regola 2 è il motivo per cui il tag non è un'ottimizzazione ma una
necessità. La union dà i layout sovrapposti; il tag è l'unica cosa che sa
quale è vivo. Insieme, nella stessa struct, sono una **tagged union** — la
struttura dati centrale di tutto il progetto: nodi dell'AST, token, valori a
runtime della VM.

La disciplina è a carico del programmatore: **il tag va guardato prima di ogni
accesso alla union**. Non c'è compilatore né sanitizer che copra l'errore.

Nota: "ospita un membro alla volta" è un contratto, non un divieto. Il C
lascia scrivere tutti i membri che vuoi, uno dopo l'altro; ognuno distrugge il
precedente, perché i byte sono gli stessi. È fisica della memoria, non una
regola del linguaggio.

### 4.3 Allineamento, misurato

Previsione: 4 di tag + 16 di union = 20. Misura con `sizeof` e `offsetof`:

```
sizeof  = 24
offsetof(union) = 8

offset  0     4        8                       24
        +-----+--------+------------------------+
        | tag |  PAD   |        union (16)      |
        +-----+--------+------------------------+
                4 byte buttati: la union contiene double e puntatori,
                quindi è allineata a 8, e 4 non è multiplo di 8
```

L'allineamento non è un capriccio: la CPU carica 8 byte in un colpo solo se
l'indirizzo è multiplo di 8. Su x86-64 un accesso disallineato è legale ma più
lento, su altre architetture il programma muore.

Quei 4 byte non si recuperano. Mettendo il tag dopo la union si ottiene
padding **in coda**, perché la dimensione di una struct deve essere multipla
del suo allineamento — altrimenti in un array il secondo elemento partirebbe
disallineato. Sempre 24, anche con un tag da 1 byte.

| Layout | `sizeof` | Costo del tag |
|---|---|---|
| Struct grassa, campi in fila | 40 | 0 (entrava nel padding in coda) |
| Tag + union | **24** | 8 (4 di tag + 4 di padding) |

Il tag si paga caro e si risparmia comunque il 40%, sulla struttura dati più
numerosa dell'interprete.


## 5. La grammatica — dove vive, esattamente, la precedenza

La domanda rimasta aperta dalla sessione 1: `*` lega più forte di `+`, ma
**dove sta scritto**? Non nel lexer, che vede caratteri e non sa cosa sia
un'espressione. Non nell'interprete, che riceve un albero già formato e lo
visita dal basso senza decidere niente. La risposta è: nella grammatica — e
non come annotazione, ma come **forma delle regole**.

### 5.1 BNF, la notazione

Backus e Naur, 1959, per ALGOL 60. Prima non esisteva un modo preciso di
scrivere "ecco com'è fatto un programma valido".

È un **meta-linguaggio**: un linguaggio per descrivere linguaggi. Nelle regole
convivono due strati di simboli, e vanno tenuti separati o non si capisce
niente.

Simboli della notazione — punteggiatura della BNF, non appartengono a mango:

| Simbolo | Si legge |
|---|---|
| `->` | "può essere" (anche scritto `::=`) |
| `\|` | "oppure" |
| accostamento | "seguito da": `a b c` = `a`, poi `b`, poi `c` |

Simboli del linguaggio descritto — questi sì, sono roba di mango:

| Tipo | Convenzione | Esempi | Cosa sono |
|---|---|---|---|
| **terminali** | maiuscolo o fra apici | `NUM`, `'+'`, `'('` | i token che sputa il lexer: la derivazione finisce qui |
| **non terminali** | minuscolo | `expr`, `term` | nomi inventati da te per categorie di costrutti, mai presenti nell'input |

Tre regole strutturali:

1. A sinistra della freccia c'è **un solo non terminale**, sempre. È questo che
   rende la grammatica *libera dal contesto*: come si espande `term` non
   dipende da cosa gli sta intorno.
2. Ogni non terminale usato a destra deve avere una sua regola, altrimenti è
   un buco. E se nessuno lo nomina, è irraggiungibile: codice morto.
3. C'è un **simbolo iniziale** da cui parte ogni derivazione. Qui è `expr`.

### 5.2 Derivare è costruire l'albero

Derivare = partire dal simbolo iniziale e sostituire, un passo alla volta, un
non terminale con una delle sue alternative, finché restano solo terminali. A
ogni passo si riscrive la **stringa intera**: niente si perde, niente compare
dal nulla.

```
expr                      simbolo iniziale
expr '+' expr             applicata  expr -> expr '+' expr
NUM  '+' expr             applicata  expr -> NUM   sul primo
NUM  '+' NUM              applicata  expr -> NUM   sul secondo
 1   +   2                combacia coi token in mano
```

Ogni passo di sostituzione **è un nodo**, e i simboli a destra della freccia
sono i suoi figli. La derivazione non descrive l'albero: **è** l'albero.

> Derivare è un'operazione **puramente sintattica**. Non si calcola niente,
> mai. Il valore non esiste ancora — esisterà alla Tappa 3, quando
> l'interprete visiterà l'albero. Qui si decide solo *che forma ha*.

### 5.3 L'ambiguità, e perché non è un problema estetico

La grammatica ingenua, quella che viene in mente per prima:

```
expr -> expr '+' expr
      | expr '*' expr
      | NUM
```

`1 + 2 * 3` si deriva in due modi, scegliendo un'alternativa diversa al primo
passo:

```
A                                B
expr                             expr
expr '*' expr                    expr '+' expr
expr '+' expr '*' expr           NUM  '+' expr
NUM  '+' expr '*' expr           NUM  '+' expr '*' expr
NUM  '+' NUM  '*' expr           NUM  '+' NUM  '*' expr
NUM  '+' NUM  '*' NUM            NUM  '+' NUM  '*' NUM
 1   +    2   *    3              1   +    2   *    3
```

Stessa stringa finale, alberi diversi:

```
A          (*)                 B        (+)
          /   \                        /   \
        (+)   (3)                    (1)   (*)
       /   \                              /   \
     (1)   (2)                          (2)   (3)
```

A vale **9**, B vale **7**. Entrambi legali secondo quelle tre righe.

Questa grammatica non è "un po' imprecisa": **non definisce il significato del
programma**. Dato `1 + 2 * 3` non esiste una risposta giusta, esistono due
alberi e la scelta è un capriccio dell'implementazione. Si dice che la
grammatica è **ambigua**.

E qui cade la domanda aperta: la gerarchia fra `*` e `+` non è sparita da
qualche parte — **non è mai stata scritta**. Il posto dov'era prevista era
vuoto.

### 5.4 La stratificazione: un livello per ogni forza di legame

L'albero A ha un `+` **sotto** un `*`. Per renderlo irraggiungibile serve che
le cose ammesse sotto un `*` siano *meno* di quelle ammesse sotto un `+`. Un
non terminale solo, `expr`, non può denotare due insiemi diversi: servono due
nomi.

- un `term` è una catena di **fattori** legati da `*` — un `+` dentro un
  `term` non può esistere;
- un `expr` è una catena di **term** legati da `+`;
- ogni `term` è un `expr`; non ogni `expr` è un `term`.

L'idioma per "catena di X legati da un separatore", che serve a ogni livello:

```
lista -> parola
       | lista ',' parola
```

*"un `lista` è **una parola sola**, oppure **una lista più corta**, una
virgola, e un'altra parola."* Il caso base non ricorsivo viene prima; il
membro della catena è **lo stesso non terminale in tutte e due le
alternative** — è lui che fa toccare terra.

Applicato due volte, più il livello delle parentesi, dà la grammatica
aritmetica di mango:

```
expr   -> term
        | expr '+' term

term   -> factor
        | term '*' factor

factor -> NUM
        | '(' expr ')'
```

### 5.5 Perché ora l'albero A è irraggiungibile

Per avere un `+` sotto un `*` servirebbe che il figlio sinistro di un `*`
contenga un `+`. Nella regola di `term`, a sinistra di `'*'` può stare solo
`term`; e `term` produce `factor` o `term '*' factor`. **La parola `expr` non
compare da nessuna parte dentro `term`** — se non dietro una parentesi.

Non è scoraggiato: non esiste nessuna sequenza di sostituzioni che ci arriva.

E l'albero B si deriva in un modo solo — a ogni riga c'è una sola scelta
possibile:

```
expr
expr '+' term              unica alternativa che produce un '+'
term '+' term
factor '+' term
NUM  '+' term
NUM  '+' term '*' factor   unica alternativa che produce un '*'
NUM  '+' factor '*' factor
NUM  '+' NUM  '*' NUM
 1   +    2   *    3
```

> `*` lega più forte di `+` **perché `term` sta un livello sotto `expr` nelle
> regole**. Non c'è una tabella di priorità, non c'è un `if` nel parser, non
> c'è un numero associato a un operatore. C'è che uno dei due non terminali
> nomina l'altro e l'altro non ricambia.

**La precedenza è la profondità nella gerarchia delle regole**, e diventerà la
profondità nell'albero — che è la conclusione della sessione 1.

Corollario operativo per la Tappa 2: **un livello di precedenza = un non
terminale**. Aggiungere `^`, che lega più forte di `*`? Si infila un non
terminale fra `term` e `factor`. Nient'altro cambia.

### 5.6 L'associatività è la forma della ricorsione

La stessa riga che codifica la precedenza codifica anche l'associatività, e lo
fa con la posizione del non terminale ricorsivo:

```
expr -> expr '+' term      ricorsione a SINISTRA  ->  1-2-3 = (1-2)-3 = -4
expr -> term '+' expr      ricorsione a DESTRA    ->  1-2-3 = 1-(2-3) =  2
```

`+ - * /` vogliono la prima forma. `=` e `^` vogliono la seconda.

Verificato su `8 / 4 / 2`, che deve fare **1**, affiancando le due forme:

```
  term -> term '/' factor              term -> factor '/' term
  (ricorsione a SINISTRA)              (ricorsione a DESTRA)

           (/)  <- radice                       (/)  <- radice
          /   \                                /   \
        (/)   (2)                          (8)    (/)
       /   \                                     /   \
     (8)   (4)                                 (4)   (2)

  8/4 = 2, poi 2/2 = 1   CORRETTO        4/2 = 2, poi 8/2 = 4   SBAGLIATO
```

La ricorsione a sinistra costringe il sottoalbero a crescere a sinistra,
perché il lato destro di ogni `/` è un `factor` — un singolo numero o una
parentesi, mai una catena. La catena può accumularsi solo dall'altra parte.
**L'associatività a sinistra è gratis**: non c'è niente da scrivere per
ottenerla, e niente da sbagliare per distrazione.

Inversione da tenere a mente, perché confonde:

- la **derivazione** va dall'alto in basso — il primo nodo prodotto è la
  **radice**;
- la **valutazione** va dal basso in alto — la radice è l'**ultima**
  operazione eseguita.

In `8 / 4 / 2` il primo `/` che compare nella derivazione è quello *più a
destra* nel testo. Il `/` che scrivi per secondo è il primo che si calcola.

**Trappola già visibile, che esplode alla Tappa 2:** la ricorsione a sinistra
manda in loop infinito un parser a discesa ricorsiva — la funzione `expr()`
chiamerebbe `expr()` come primissima cosa, senza aver consumato un token. La
grammatica si riscriverà con un'iterazione, `term ('+' term)*`, che genera lo
stesso linguaggio e raggruppa a sinistra. Problema rimandato, non risolto.

### 5.7 Le parentesi chiudono il cerchio

`factor` è il livello più basso, e fra le parentesi richiama `expr`, il più
alto:

```
   expr  --nomina-->  term  --nomina-->  factor
     ^                                      |
     +------------- '(' expr ')' -----------+
```

La gerarchia **non è lineare: è un ciclo**. Ed è esattamente la frase della
sezione 2 di questo README — *"le parentesi non aggiungono un livello,
scambiano chi sta sopra"* — vista dal lato delle regole: dentro una parentesi
si riparte dal simbolo iniziale, quindi lì dentro si può scrivere di nuovo
qualunque cosa, `+` compresi. Le parentesi sono l'unico modo di far scendere
un `+` sotto un `*`.

E non lasciano traccia nell'albero perché sono consumate dalla regola di
`factor`: il nodo prodotto è quello che stava **dentro**.

### 5.8 Il nucleo aritmetico completo

`-` e `/` **non richiedono livelli nuovi**: legano con la stessa forza dei
compagni che hanno già un livello. Lo dimostrano due valori — `1 - 2 * 3` fa
`-5` e non `-3`, quindi `*` lega più forte di `-`, quindi `-` sta con `+`;
`8 / 4 + 1` fa `3` e non `1.6`, quindi `/` sta con `*`.

Aggiungere un operatore alla stessa forza di legame = **aggiungere
un'alternativa a un livello esistente**, non un livello.

```
expr   -> term
        | expr '+' term
        | expr '-' term

term   -> factor
        | term '*' factor
        | term '/' factor

factor -> NUM
        | '(' expr ')'
```

Questo è il nucleo aritmetico di mango, ed è il contratto delle Tappe 1 e 2.

---

## Rimasto aperto

- **`grammatica.md` è da scrivere.** La sezione 5 copre solo il nucleo
  aritmetico `+ * ()`. Manca la grammatica completa di mango: `-` e `/` (a
  quale livello vanno, e perché non serve inventarne di nuovi), e soprattutto
  il **meno unario**, che è una decisione di design: `-2 * 3` deve dare l'albero
  `(-2) * 3` o `-(2 * 3)`? Va deciso e motivato.
- **Dimensione e allineamento sono due proprietà indipendenti.** Confuse tre
  volte nella stessa sessione. `_Alignof(aggregato)` = il massimo degli
  `_Alignof` dei membri, ricorsivamente fino agli **scalari** — non si deduce
  dalla `sizeof` di nessuno. `sizeof(aggregato)` invece dev'essere un multiplo
  di quell'allineamento. Coincidono solo sugli scalari (`int` 4/4, `double`
  8/8, puntatore 8/8), ed è per questo che la confusione è facile: divergono
  solo sugli aggregati (`union NodeValue` 16/8, `struct { char c[100]; }`
  100/1).
- `sizeof` ritorna un `size_t`, non un `int`: lo specificatore `printf` giusto
  è `%zu`. `%lu` funziona qui per coincidenza (su x86-64 Linux `size_t` *è*
  `unsigned long`) e stampa spazzatura su una piattaforma a 32 bit.
- `offsetof` prende **un tipo e un nome di campo**, non un'espressione: è
  risolta a compile time, non c'è nessun oggetto da dereferenziare.

## File

- `pre_work.c` — l'esperimento di misura: `sizeof`, `offsetof` e `_Alignof`
  sulla prima tagged union, più la lettura del membro sbagliato di una union
  che finisce in segfault. Non è codice di mango, è uno strumento di misura,
  tenuto come traccia di come ci si è arrivati.
- `grammatica.md` — **da scrivere**: la grammatica BNF completa di mango, con
  un livello per precedenza, l'associatività annotata riga per riga, la scelta
  motivata sul meno unario, e due esempi derivati a mano col rispettivo albero
  (uno dei quali con parentesi). È il contratto delle Tappe 1 e 2.

## Confronto col sorgente vero

<da scrivere dopo la Tappa 1, leggendo come clox rappresenta i suoi valori>
