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

---

## Rimasto aperto

- **La grammatica**: dove vive esattamente la regola per cui `*` lega più
  forte di `+`? Non nel lexer, non nell'interprete. Capire dove significa
  capire come il parser la applicherà.
- `sizeof` ritorna un `size_t`, non un `int`: lo specificatore `printf` giusto
  è `%zu`. `%lu` funziona qui per coincidenza (su x86-64 Linux `size_t` *è*
  `unsigned long`) e stampa spazzatura su una piattaforma a 32 bit.
- `offsetof` prende **un tipo e un nome di campo**, non un'espressione: è
  risolta a compile time, non c'è nessun oggetto da dereferenziare.

## File

- `pre_work.c` — l'esperimento di misura di questa sessione: `sizeof` e
  `offsetof` sulla prima tagged union. Non è codice di mango, è uno strumento
  di misura, tenuto come traccia di come ci si è arrivati.

## Confronto col sorgente vero

<da scrivere dopo la Tappa 1, leggendo come clox rappresenta i suoi valori>
