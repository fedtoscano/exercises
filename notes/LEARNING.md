# Percorso

Registro di studio. Lo leggo all'inizio di ogni sessione e lo aggiorno alla
fine, prima del commit. È scritto per te, non per me.

**Traccia attiva:** **mango** — un linguaggio: interprete → VM bytecode → GC — Tappe 0 e 0-bis **chiuse**, Tappa 1 (lexer) aperta: teoria fatta, codice non ancora
**Ultima sessione:** 2026-09-22 — richiamo a freddo chiuso, poi teoria del lexer
**Sessioni fatte su questa tappa:** 1 (Tappa 1)

---

## Stato

**Fatto finora**
- Fondamenta C (mar–mag 2026): liste singole e doppie, array dinamico con
  raddoppio di capacità e map/filter con puntatori a funzione, allocatore su
  `sbrk` con header di blocco e free list first-fit, n-queens con backtracking.
- Note a mano su rappresentazione binaria: complemento a uno, complemento a
  due, operatori bitwise (`notes/binary/`).
- Trilogia server TCP (24–28 ago 2026): echo → `fork` → `pthread` + mutex →
  `poll()`. Sul thread server hai visto una race distruggere 291218 update su
  300000 *prima* di sentire nominare `pthread_mutex_t`.
- Audio (4 set 2026, `~/Work/audio`): writer WAV scritto a mano — header
  RIFF/WAVE byte per byte, little-endian esplicito — prima 2s di silenzio
  (`silence.c`), poi una sinusoide a 440 Hz (`sine.c`).
- Toolchain: ASan+UBSan di default, `-Wconversion`, `-MMD -MP`, con
  `make SAN=` come via di fuga documentata. Era a posto prima della prima riga
  di codice vero.
- **Tappa 0, sessione 1 (17 set 2026):** la catena testo → token → albero →
  valore, e perché l'albero esiste (una lista è lineare, non può contenere la
  precedenza). Associatività: `+ - * /` associano a sinistra, e te lo impone
  `8/4/2`, non una convenzione arbitraria. Poi il nodo dell'AST: arietà
  variabile → non esiste una struct sola che vada bene → serve un campo **tag**
  esplicito, perché "quali campi sono validi" non è deducibile dai byte. Da lì
  `union`, `tagged union`, e allineamento/padding misurati a mano con `sizeof`
  e `offsetof`: struct grassa 40 byte, tag + union 24, union all'offset 8.

- **Tappa 0, sessione 2 (17 set 2026):** la grammatica. BNF come
  *meta*-linguaggio, con i due strati di simboli da non confondere (`|` è
  punteggiatura della notazione, `'+'` è un carattere di mango). Derivare è
  puramente sintattico — sbagliando hai valutato entrambi gli alberi di
  `1 + 2 * 3` e ottenuto **9 e 7**, che è la dimostrazione migliore possibile
  del perché una grammatica ambigua non è un difetto estetico: non definisce
  il significato del programma. Poi la stratificazione `expr / term / factor`,
  derivata da te dopo parecchie sbandate: **la precedenza è la profondità
  nelle regole**, l'associatività è la posizione del non terminale ricorsivo,
  e `factor -> '(' expr ')'` chiude il ciclo dal livello più basso al più
  alto, che è il motivo per cui le parentesi "scambiano chi sta sopra".

- **Tappa 0, sessione 3 (18 set 2026):** chiuse le due decisioni rimaste
  aperte, e con un metodo nuovo.

  `addop -> '+' | '-'` scartato: sarebbe l'unico non terminale che **non
  ritorna un nodo**. Nell'albero di `1 + 2` ci sono tre nodi e nessuno è il
  `+`: l'operatore non è un nodo, è il **tag** del nodo binario — un campo,
  come il tag della tagged union della sessione 1. Un non terminale che
  corrisponde a un campo rompe la corrispondenza grammatica↔albero, che alla
  Tappa 2 è l'unica bussola.

  Meno unario. `-2 + 3` (**1** contro **-5**) uccide l'ipotesi "in cima a
  `expr`": un prefisso che lega più debolmente della somma si mangia
  l'addizione a destra. Restavano due posizioni — dentro `factor`, oppure un
  livello `unary` a sé — e qui sta il salto della giornata: invece di cercare
  un controesempio hai dimostrato che **non può esistere**. Due posizioni
  danno alberi diversi solo se fra loro vive un operatore che possa restare
  *intrappolato*; fra `factor` e `unary` non c'è niente. Grammatiche
  equivalenti. Primo argomento di non-esistenza del percorso, ed è un tipo di
  argomento più forte del controesempio.

  Scelto `unary` a sé, non perché generi alberi diversi (non lo fa) ma per la
  Tappa 2: `expr` e `term` sono ricorsivi a sinistra → cicli; `unary` è
  ricorsivo a destra → si richiama; `factor` smista e basta. Quattro forme
  diverse, una per funzione. Grammatica più lunga, codice più semplice.

La grammatica di mango è chiusa. È il contratto delle Tappe 1 e 2:

```
expr   -> term  | expr '+' term  | expr '-' term
term   -> unary | term '*' unary | term '/' unary
unary  -> '-' unary | factor
factor -> NUM | '(' expr ')'
```

- **Tappa 0, sessione 4 (21 set 2026):** `grammar.md` chiuso, e la Tappa 0 con
  lui. Il richiamo a freddo è diventato la lezione, ed era la parte migliore.

  Da una derivazione difettosa che ti ho messo davanti hai visto il salto
  `term -> NUM` che scavalca `unary`, ma **non** il secondo difetto nello
  stesso passo: due non terminali espansi insieme. La tua riscrittura lo
  faceva già giusto — sai farlo, non lo riconosci ancora quando è scritto da
  altri. Da lì la tua domanda migliore della sessione: *in che ordine si
  espandono?* Ci sei arrivato da solo: **l'albero di derivazione è invariante
  rispetto all'ordine**, cambia solo quando tocchi i nodi, non dove stanno.
  Da lì i nomi — derivazione canonica sinistra e destra — e il motivo per cui
  contano: un parser a discesa ricorsiva *è* una derivazione canonica
  sinistra costruita al contrario, ed è da lì che viene la sigla **LL**.

  Nominata anche la distinzione che ti mancava: l'albero che avevi disegnato
  era un **albero di derivazione**, non l'AST. Su `(1+2)*3` sono 21 nodi
  contro 5. `expr`/`term`/`unary`/`factor` sono impalcatura: impongono la
  precedenza durante il parsing e poi spariscono, perché la precedenza ormai
  è nella forma.

  Chiusa la terza forma di ricorsione: `unary` termina perché **consuma il
  `-` prima di richiamarsi**, quindi l'input residuo si accorcia a ogni giro.
  La ricorsione a sinistra non consuma niente prima di richiamarsi, e per
  questo non ha fondo. L'hai tracciato tu su `- - 5`.

  Allineamento: quarto giro, e stavolta *«un array è allineato come il suo
  elemento»* è uscita da te. La regola generale però non è ancora tua — l'hai
  formulata come **minimo comune multiplo** invece che **massimo**. Danno
  zero in pratica (gli allineamenti sono sempre potenze di 2, quindi lcm e
  max coincidono sempre), ma è un modello più complicato del necessario, e lo
  è perché ti mancava il vincolo che rende sufficiente il massimo.

  **Come è finita la sessione, detto com'è.** Dopo un'ora di richiamo hai
  chiesto due volte che scrivessi io `grammar.md`. La prima volta hai scelto
  la via di mezzo — un livello a me come modello, tre a te. Alla seconda hai
  usato l'opt-out pieno, e l'ho eseguito: **le 459 righe di `grammar.md` sono
  scritte da me, non da te.** Il file è buono ma non è un tuo artefatto, e
  alla Tappa 2 lo leggerai come roba di qualcun altro.

  **La diagnosi vera è tua, ed è più a monte della mia.** Avevo scritto che il
  problema era la dimensione della consegna. Tu hai detto un'altra cosa: che
  fin dal richiamo a freddo ti ho bombardato di domande sulle **differenze fra
  implementazioni** della grammatica, mentre non avevi chiaro a cosa serve una
  grammatica, cos'è un operatore, cosa vuol dire che uno lega più di un altro,
  cosa sono i livelli, cosa vuol dire associatività. Hai ragione, e si vede nel
  transcript: due volte hai segnalato che la base mancava — *«spiegami meglio
  la domanda, io ne ho scritta solo una»* e *«spiegami meglio cosa intendi per
  associatività»* — e tutte e due le volte ho risposto alla singola domanda e
  ho tirato dritto, invece di fermare il piano.

  Una domanda comparativa presuppone che l'oggetto confrontato esista già.
  Se non esiste non è *difficile*: è **incomprensibile**, e sembra un problema
  di rigore quando è un problema di fondamenta. Regola nuova: niente domande
  A-contro-B finché non sai nominare e usare l'oggetto da solo.

- **Tappa 0-bis, sessione 1 (21 set 2026, stesso giorno):** ripartiti dal
  vocabolario, da zero, una domanda alla volta. Niente file, niente consegne.

  Da `2 + 3 * 4` hai costruito tu, in ordine: che esistono **due sequenze
  legali** con due risultati (14 e 20); che **nessun carattere della stringa**
  dice quale scegliere — le parentesi sarebbero l'unico, e non ci sono —
  quindi l'informazione sta nella macchina, non nell'input. Da lì:

  - **operando**: i numeri che un operatore si prende;
  - **precedenza**: la regola fissata sui simboli, che decide chi vince
    l'**operando conteso**. In `2 + 3 * 4` il conteso è il 3, e lo vince `*`;
  - **«legare più forte»** = prendersi l'operando conteso. Non una metafora;
  - **livello**: gli operatori che legano uguale. `+ -` sopra, `* /` sotto;
  - **associatività**: la seconda regola, che serve dove la precedenza tace,
    cioè **dentro un livello**. Su `10 - 4 + 3` (9 contro 3) hai visto tu che
    la precedenza non sa rispondere, e hai detto «si segue da sinistra a
    destra» prima che la parola fosse nominata.

  Poi la tua domanda, che è il cardine del progetto e l'hai fatta tu: *«più si
  scende in basso, più gli operatori legano forte?»*. Sì. E le tre facce della
  stessa frase: lega più forte = si prende il conteso = si esegue prima = sta
  più in basso nell'albero. Da lì anche la correzione al tuo «livelli
  dell'AST»: i livelli sono della **grammatica**, l'albero ha una profondità
  che ne è la conseguenza.

  Chiuso con `^`: collocato da te su un livello nuovo sotto `*`. Il pezzo
  saltato, e il più importante: `2 + 3 ^ 2 = 11` **non dimostra niente** (11
  esce anche mettendo `^` col `*`), la dimostrazione è `2 * 3 ^ 2 = 18` contro
  i 36 dell'ipotesi alternativa. Criterio generale: **una stringa dimostra solo
  se le due ipotesi danno due numeri diversi**; altrimenti è muta anche se la
  conclusione è giusta.

  **Seconda metà della stessa sessione — albero e grammatica, costruiti da
  te.** Da `2 + 3 * 4` e dalla tua sequenza `3*4=12` / `2+12=14`: hai
  sostituito il `12` con l'operazione che l'ha prodotto e hai scritto
  `2 + (3 * 4)` — cioè hai messo tu le parentesi che all'inizio avevi detto
  «non ci sono». Da lì:

  - **il mestiere del parser**: prendere una stringa dove la struttura è
    *implicita* (nascosta nella tabella dei livelli) e renderla *esplicita*;
  - **dove sta un numero può stare un'operazione intera** — il `+` non
    distingue `12` da `(3*4)`. È l'annidamento, ed è la radice della
    ricorsione nella grammatica;
  - **l'albero**: disegnato da te. Primo tentativo con un pallino di troppo,
    proprio dove stavano le parentesi. Le parentesi **non sono un nodo**: sono
    un'istruzione al parser, spariscono. (Ridedurle dall'albero è il viaggio di
    ritorno, un'altra cosa.)
  - **livello ≠ profondità**, e questa chiude il buco di stamattina. Il livello
    è dell'**operatore**, fissato una volta per tutto il linguaggio: `+` è
    debole sempre. La profondità è di **quell'albero lì** e cambia: hai
    disegnato `3*4+2` e `(3+4)*2` e visto il `+` una volta sopra e una volta
    sotto. Le parentesi sono l'unico strumento che fa disaccordare le due.
    Da qui in poi: **non esistono "i livelli dell'AST"**.

  Poi la grammatica, tirata fuori dalla nozione di **catena**: su
  `2 + (3*4) - 5` hai contato tre anelli, non quattro numeri; hai visto che un
  anello non è un numero ma «una cosa del livello sotto»; e che *«oppure un
  numero»* non è un caso a parte, perché un numero è una catena da un anello
  solo con zero operatori. Con quello la struttura è collassata in quattro
  frasi di italiano, scritte da te (la 1 su mio stampo, le altre tre tue, la 3
  corretta due volte: l'operando del `-` è un **unario**, non un numero — e la
  prova è `- -5`, la stessa che avevi tracciato al mattino).

  Chiusura: le tue quattro frasi affiancate alle quattro righe BNF di
  `grammar.md`. Sono **le stesse**, in notazione più corta. `->` è "è", `|` è
  "oppure", l'accostamento è "seguito da"; e "catena" si dice con due
  alternative — la più corta è un anello solo, una più lunga è una catena più
  un anello. Ultimo regalo: la catena cresce **a sinistra** del `+`, quindi
  l'associatività a sinistra non è una regola scritta da nessuna parte, è una
  conseguenza di dove sta la parola `expr` in quella riga. Cioè la tua frase
  del mattino — «nessuno lega più forte, quindi si segue da sinistra a destra»
  — arrivata dall'altro capo.

  **Il file che al mattino non riuscivi a leggere, alla sera lo hai
  riscritto.**

  **Verifica finale, 6 domande: 4 piene, 2 errori.** Giuste: operando conteso
  e precedenza su `6 - 2 * 3`; l'albero e il valore di `-(2 + 3) * 4` (-20),
  nodo unario a un figlio compreso; la lettura in italiano di
  `term -> unary | term '*' unary`; la scelta di `1 * 2 OP 3` come stringa che
  colloca un operatore nuovo.

  **Rimaste aperte, da riprendere a freddo domani — sono il richiamo:**

  1. `((((5))))` — hai detto 4 nodi. Quanti operatori ci sono in quella
     stringa? Le parentesi non sono nodi: è lo stesso errore del pallino di
     troppo nel primo disegno, ricomparso tre ore dopo.
  2. `term -> unary | term '*' unary` — hai detto che la catena cresce **a
     destra**. Quale dei due simboli a destra della freccia ripete il nome
     della riga? Secondo inciampo della giornata sullo stesso passo: al
     mattino avevi detto «il non terminale ricorsivo è sempre a sinistra»
     senza guardare, ora «a destra» senza guardare. L'errore non è la
     direzione, è che non identifichi **quale** simbolo è quello ricorsivo.
  3. «Il `-` in `(8 - 2) / 3` ha cambiato livello?» — hai risposto sulla
     **profondità**. Riformulata: esiste una qualunque espressione di mango in
     cui `-` non sta sul livello debole?
  4. Perché `1 + 2 OP 3` è muta: non "perché il `+` sta un livello sopra", ma
     perché sotto **tutte e due** le ipotesi succede la stessa cosa e viene lo
     stesso numero.

- **Tappa 0-bis chiusa + Tappa 1 sessione 1 (22 set 2026):** un'ora di
  richiamo a freddo sulle quattro domande lasciate aperte, poi la teoria del
  lexer. Zero codice, per scelta.

  **Richiamo, esito secco.** 1 e 3 piene — `((((5))))` ha un nodo solo (era
  sbagliata ieri), e il `-` sta sul livello debole in *ogni* espressione di
  mango. La 2: il simbolo ricorsivo l'hai nominato giusto (`term`) col
  **motivo sbagliato** — «è quello che può essere due cose», che è
  l'**alternanza**, non la ricorsione. Applicato a
  `factor -> NUM | '(' expr ')'` il tuo criterio dichiara `expr` ricorsivo, e
  hai detto di sì. Riparato meccanicamente: copri la destra, leggi il nome a
  sinistra della freccia, cercalo a destra. **Una riga è ricorsiva quando
  nomina sé stessa.** Poi la direzione l'hai *derivata* invece di indovinarla
  (terza volta che la domanda tornava): la radice di `2 * 3 * 4` è il `*` più
  a destra.

  **La cosa migliore della mattinata è un tuo errore che hai riparato tu.**
  Da lì hai generalizzato: «la radice è sempre l'operatore più a destra».
  Falsa — su `2 + 3 * 4` la radice è il `+`, che sta a sinistra. In due passi
  hai trovato la clausola mancante (stesso livello) e poi il verso giusto: a
  livelli diversi la radice è quella sul livello **debole**. Forma finale:
  **la radice è l'operatore più debole; a parità di livello, il più a
  destra.** E si riaggancia a ieri — chi *vince l'operando conteso* è il
  forte, ma chi finisce in **cima** è il debole. Due «vittorie» opposte, la
  parola era la stessa.

  La 4: forma giusta («viene lo stesso numero sotto tutte e due»), **ipotesi
  sbagliate** — avevi messo `^` col `+`, e quelle due ipotesi danno 27 contro
  9, cioè la stringa parlerebbe. Ricostruito: le ipotesi vere erano `^` col
  `*` contro `^` sotto `*`, e `2 + 3 ^ 2` fa 11 sotto entrambe. Generalizzato
  in una regola nuova: **una stringa distingue due ipotesi solo se contiene
  gli operatori di cui le ipotesi parlano.** Senza un `*` nella stringa, `^`
  contro `*` non è decidibile.

  **Teoria della Tappa 1.** Partiti dal conto: `12 + 345` sono **8 caratteri**
  e **3 simboli** di grammatica. I cinque mancanti hai detto tu dove vanno, e
  con le parole giuste — i due mestieri del lexer sono **scartare** (gli
  spazi, che nessuna regola produce) e **raggruppare** (le cifre in un numero).

  La regola di raggruppamento è arrivata in tre giri, ed è il pezzo tecnico
  della giornata. Prima «finché non incontra uno spazio» → rotta da `12+345`.
  Poi «finché non trova un operatore» → rotta da `(12)` e dalla fine del file.
  Poi la riformulazione, ed è tua: **«continuo finché il carattere è una
  cifra»**. Elencare i terminatori ti obbliga a conoscere tutto il resto del
  linguaggio e la lista si allunga a ogni operatore nuovo; la condizione
  positiva guarda solo il carattere in mano, e le cifre sono dieci per
  sempre. Anche l'EOF smette di essere un caso a parte, perché non è una
  cifra. Nome: **maximal munch**.

  Poi il token: **tag + valore**, cioè la tagged union della sessione 1 della
  Tappa 0 che arriva per la prima volta su qualcosa che scriverai davvero. Il
  `+` non ha valore, il `12` sì — quindi il tag non è un'etichetta
  descrittiva, è il **permesso di leggere l'altro campo**.

  **Il pezzo concettuale grosso: il meno unario non è un tag.** Lo avevi messo
  nell'inventario. Il lexer vede lo stesso byte `0x2D` in `5 - 3` e in `- 5`;
  la tua regola — «se prima non c'è una cifra è unario» — l'hai vista cadere
  su `(1 + 2) - 3`. Rattopparla («cifra oppure `)`») funzionerebbe pure, ma
  significa chiedere al lexer di sapere dove si trova nella grammatica. E la
  distinzione **esiste già**: `-` compare in due righe diverse, `expr '-' term`
  e `'-' unary`. È posizionale, quindi è del parser. Confine della Tappa 1:
  **il lexer non conosce la grammatica e non si ricorda cosa è venuto prima.**
  Quando ti viene voglia di dargli memoria del contesto, è il parser che bussa
  alla porta sbagliata.

  Chiuso con l'**EOF come token sentinella**. Design A (token con tag suo)
  contro design B (`NULL` a fine file): in B ogni punto del parser che guarda
  un token deve prima controllare il puntatore — decine di posti, e
  dimenticarne uno è un segfault, non un errore di sintassi. A **elimina** il
  caso speciale invece di gestirlo bene. Stessa tecnica del nodo fittizio
  nelle liste concatenate, che avevi già scritto ad aprile.

  Inventario finale, ricomposto da te con tutte e tre le correzioni dentro:
  **numero, +, -, *, /, (, ), EOF — otto tag.**

  **Coda della sessione: il carattere illegale, che era la domanda di domani e
  l'hai risolta oggi.** Su `12 @ 34` hai proposto un **token di errore** con
  tag suo — è la soluzione giusta, ed è quella di clox: il vincolo «il parser
  riceve sempre un token» resta vero, e fine-file ed errore diventano due tag
  invece di due meccanismi.

  Poi il messaggio d'errore. Prima versione: *«il token che hai inserito è
  sbagliato: @»*. Due difetti. Il vocabolario — **l'utente non inserisce
  token, scrive caratteri**; il token è il prodotto del lexer, e `@` è
  esattamente il carattere che non è riuscito a diventarne uno. E la sostanza:
  su un file di 200 righe quel messaggio non serve a niente, mancano file,
  riga e colonna.

  Il pezzo migliore è stato smontare *«riga e colonna le so già»*. Con un
  buffer disegnato e il puntatore sul `@`, hai visto che da un puntatore e un
  byte la riga **non si sa**: l'unica cosa gratis è l'**offset**, perché è il
  puntatore stesso. Riga e colonna sono **stato che il lexer mantiene** —
  riga +1 a ogni `\n`, colonna azzerata lì e incrementata altrove. Non si
  cercano dopo, si contano durante.

  Chiusa con la decisione di progetto: la posizione va in **ogni token**, non
  solo in quello di errore. Motivazione tua, ed è quella vera: alla Tappa 2 il
  parser dovrà dire *«hai aperto questa parentesi e non l'hai chiusa»*
  puntando a un `(` incontrato venti token prima — o quella posizione se l'è
  portata dietro il token, o è persa.

  Token di mango, forma finale della giornata: **tag + valore + posizione.**

  **Anche l'ultima domanda l'hai voluta subito.** Primo tentativo: «decide il
  lexer, e si ferma». Caduto in due mosse. La prima empirica, e la conosci già
  senza saperlo: `gcc` su un file con tre errori te ne mostra **tre**, non uno
  — fermarsi al primo costa una ricompilazione per errore, e il tempo
  dell'umano vale più del lavoro sprecato dalla macchina.

  La seconda è strutturale e l'hai detta tu in cinque parole — *«niente, lo
  richiamo io quando voglio»*. **Il lexer non può fermarsi perché non possiede
  il ciclo**: non ha un loop dentro, risponde a una chiamata. «Fermarsi al
  primo errore» è una **politica del chiamante**, non una proprietà del lexer.
  Il suo contratto è più modesto e più forte: *a ogni chiamata rispondo con un
  token* — dopo un errore, e a file finito (EOF per sempre).

  Terza volta nella stessa sessione che cade lo stesso confine: il lexer non
  conosce la grammatica, non si ricorda il contesto, non decide quando
  smettere.

**Per il richiamo di domani** (materiale di oggi, niente di nuovo): perché la
condizione di maximal munch è positiva e non un elenco di terminatori; perché
il meno unario non è un tag; e cosa sa **davvero** il lexer quando ha il
puntatore su un byte.

**Domande aperte**
- `sine.c:70–74` — `sine_sample` ritorna `uint16_t`, ma un campione a 16 bit è
  un intero **con segno** in complemento a due. La conversione di un `double`
  negativo a un tipo unsigned è UB, non semplicemente "wrap". Il file suona
  lo stesso: perché? Questa è la domanda, non te la risolvo io.
- `poll_server.c` ha difetti veri ancora aperti (vedi tabella sotto). La
  traccia mini-Redis che li avrebbe affrontati è parcheggiata, non cancellata.
- Traccia ML: il linguaggio in cui farla (C, Python, o Python-poi-C come hai
  già fatto con `silence.c` → `sine.c`) si decide quando ci arriviamo.

---

## Errori ricorrenti

Formato: `data ×N — sintomo — dove`. È la sorgente delle domande di ripasso
a freddo: quando un errore torna, il contatore sale e la domanda diventa più
probabile.

| Quando | ×  | Errore | Dove è successo |
|---|---|---|---|
| 2026-09-22 | 1 | Informazione creduta disponibile e invece da mantenere: «riga e colonna le so già» avendo in mano solo un puntatore in un buffer di byte. Gratis è solo l'offset | messaggio d'errore su `12 @ 34` |
| 2026-09-22 | 1 | Vocabolario dal lato sbagliato: «il **token** che hai inserito è sbagliato» detto all'utente, che ha scritto un **carattere** — il token è il prodotto del lexer | prima stesura del messaggio d'errore |
| 2026-09-22 | 3 | Risposta **nominale** al posto del meccanismo: «a condizione di maggior forza del legame», «vince il livello dell'operazione», «ci guadagna in ambiguità». Ogni volta si è sciolta appena la domanda è diventata concreta — due stringhe, due numeri | riparazione della regola sulla radice; design A/B dell'EOF |
| 2026-09-22 | 2 | «cifra» usata per «numero»: il valore del token è `12`, le cifre erano `1` e `2` — il raggruppamento serve proprio a farle sparire | informazioni del token; primo inventario dei tag |
| 2026-09-22 | 1 | Criterio di ricorsione sbagliato: «è il simbolo che può essere due cose». Quella è l'**alternanza**; applicato a `factor -> NUM \| '(' expr ')'` dichiara `expr` ricorsivo, e l'hai confermato | richiamo a freddo, domanda 2 |
| 2026-09-22 | 1 | Generalizzazione oltre il dominio: «la radice è sempre l'operatore più a destra», falsa su `2 + 3 * 4`. Vale solo a parità di livello. **Riparata da te in due passi** | subito dopo la domanda 2 |
| 2026-09-22 | 1 | Criterio di mutezza applicato alle **ipotesi sbagliate**: confrontate `^` col `+` (27 contro 9 — la stringa parlerebbe) invece di `^` col `*` contro `^` sotto `*` | richiamo a freddo, domanda 4 |
| 2026-09-21 | 2 | Due non terminali espansi in un solo passo: non riconosciuto in una derivazione difettosa scritta da altri, pur non commettendolo nella propria riscrittura | richiamo a freddo su `1 + 2` |
| 2026-09-21 | 2 | Direzione della ricorsione dichiarata senza identificare **quale** simbolo è quello ricorsivo: al mattino «sempre a sinistra» con `unary -> '-' unary` sotto gli occhi, alla sera «cresce a destra» su `term -> term '*' unary` | domanda sulle forme di ricorsione, poi verifica finale |
| 2026-09-21 | 2 | Parentesi contate come nodi dell'albero: un pallino di troppo nel primo disegno di `2 + (3*4)`, e poi 4 nodi per `((((5))))` | costruzione dell'albero, e verifica finale |
| 2026-09-21 | 1 | Risposto sulla **profondità** a una domanda sul **livello**, tre scambi dopo averli separati | verifica finale, `(8 - 2) / 3` |
| 2026-09-21 | 1 | Regola dell'allineamento formulata come **minimo comune multiplo** invece che **massimo**. Numericamente equivalente (gli allineamenti sono potenze di 2), concettualmente più pesante del necessario | ricostruzione della regola su `struct {char; double; char;}` |
| 2026-09-21 | 1 | Criterio inventato al posto della regola: «l'allineamento del campo più significativo». Non è una proprietà di un campo in C; qui dà il numero giusto per coincidenza | stessa domanda |
| 2026-09-18 | 4 | Dimensione e allineamento confusi: `_Alignof` dedotto dalla `sizeof` ("8 perché il `double` prende 8 byte"). Controprova nella stessa union: `char[16]` è il membro più grande e allinea a 1 | richiamo a freddo su `struct { char; union{double;char[16];} }` |
| 2026-09-18 | 1 | Padding calcolato come "sommo i membri e arrotondo": dà 24 per caso. Il padding stava **fra** i membri, non in coda. Su `struct {char;double;char;}` il metodo sbaglia | stessa domanda di richiamo |
| 2026-09-18 | 1 | Terminali caduti da una forma di frase: `( term )` → `term * factor`, e le parentesi "riappaiono" alla fine senza che nessuna regola le produca | derivazione di `( 4 * 5 )` |
| 2026-09-18 | 1 | Due non terminali espansi in un solo passo (`factor * factor` → `4 * 5`): la derivazione smette di essere l'albero | stessa derivazione |
| 2026-09-18 | 1 | Vocabolario di due domini mescolato: "allineamento" usato per intendere **precedenza** | domanda su cosa distingue due livelli |
| 2026-09-18 | 1 | Claim di performance senza misura ("2 alternative invece di 3, costo non indifferente"), per giunta su un modello di parser con backtracking che non è quello che scriverai | scelta su `addop` |
| 2026-09-17 | 2 | Derivazione mescolata a valutazione: calcolato `1+2` in mezzo a una derivazione sintattica | derivazioni di `1 + 2 * 3` |
| 2026-09-17 | 1 | Padding in coda dimenticato su un aggregato (`CALL_STRUCT` "12 byte") a dieci secondi dall'aver enunciato la regola giusta | sessione 2 Tappa 0 |
| 2026-09-17 | 1 | `void *` → `double *`: creduto un errore di compilazione. In C è una conversione implicita legale, zero warning anche con `-Wconversion` | `pre_work.c`, lettura del membro sbagliato della union |
| 2026-09-17 | 1 | `%d` e poi `%lu` per un `size_t`: warning zittito invece che corretto (`%zu`) | esperimento `sizeof` della Tappa 0 |
| 2026-09-17 | 1 | `offsetof(T, T->campo)`: confuso un designatore di campo con un'espressione runtime | esperimento `sizeof` della Tappa 0 |
| 2026-09-04 | 1 | `double` negativo convertito a tipo unsigned: UB, non wrap | `~/Work/audio/sine.c:70,74` |
| 2026-08-28 | 1 | Valore di ritorno di una syscall non controllato (`accept()`) | `poll-server/poll_server.c` |
| 2026-08-28 | 1 | Valore di ritorno di `poll()` ignorato, `EINTR` non gestito | `poll-server/poll_server.c` |
| 2026-08-28 | 1 | `recv()` che non distingue `EAGAIN` da errore vero | `poll-server/poll_server.c` |
| 2026-08-28 | 1 | Crescita non limitata di `nfds`: nessun tetto ai client | `poll-server/poll_server.c` |
| 2026-08-28 | 1 | `send()` bloccante dentro l'event loop | `poll-server/poll_server.c` |
| 2026-08-24 | 1 | `SO_REUSEADDR` mancante, dopo averlo citato tu stesso nel README | `echo-server/README.md` vs codice |

---

## Concetti acquisiti

Con la data, perché la ripetizione spaziata ha senso solo se so quanto è
vecchia una cosa. Un concetto entra qui quando lo hai **usato**, non quando
te l'ho spiegato.

| Data | Concetto | Verificato come |
|---|---|---|
| 2026-04-24 | Lista concatenata singola e doppia, puntatori a puntatore | `linked-list/`, metodi scritti a mano |
| 2026-04-25 | Array dinamico, raddoppio di capacità, ammortamento | `dynamic-arrays/` |
| 2026-04-26 | Puntatori a funzione: `map` / `filter` | `dynamic-arrays/` |
| 2026-05-03 | Allocatore su `sbrk`: header di blocco, free list first-fit | `my-realloc/` |
| 2026-05-18 | Backtracking ricorsivo | `n-queens/` |
| 2026-08-24 | Socket TCP: `socket`/`bind`/`listen`/`accept` | echo server + `test_client.py` |
| 2026-08-25 | `fork()`, un processo per client, zombie e `SIGCHLD` | fork server |
| 2026-08-26 | Thread POSIX, memoria condivisa | thread server |
| 2026-08-27 | Race condition e mutua esclusione | 291218/300000 update persi, poi risolti col mutex |
| 2026-08-28 | I/O multiplexing con `poll()`, un solo thread | poll server |
| 2026-09-04 | Formato RIFF/WAVE, chunk, little-endian esplicito | header WAV scritto byte per byte |
| 2026-09-17 | AST: perché la struttura ad albero è l'unica che può contenere la precedenza | alberi di `1+2*3` e `(1+2)*3` disegnati a mano |
| 2026-09-17 | Associatività a sinistra e a destra, e quali operatori la rendono visibile | `8/4/2`, `1-2-3`, `a=b=c` |
| 2026-09-17 | `union`: tutti i membri a offset 0, `sizeof` = il più grande | misurato: union di `double`+puntatore = 8, non 16 |
| 2026-09-17 | Tagged union: il tag è necessario perché "campo valido" non è nei byte | ragionato sul nodo AST ad arietà variabile |
| 2026-09-17 | Allineamento e padding, interno e in coda | `offsetof` = 8, `sizeof` = 24 contro i 20 previsti |
| 2026-09-17 | BNF: meta-simboli contro simboli del linguaggio descritto, terminali e non terminali | grammatica di mango scritta da te in sei righe |
| 2026-09-17 | Derivazione: ogni passo è un nodo, la derivazione *è* l'albero, ed è pura sintassi | derivazioni di `1 + 2 * 3` riga per riga |
| 2026-09-17 | Grammatica ambigua: stessa stringa, due alberi, due valori (9 e 7) | le due derivazioni dalla grammatica ingenua |
| 2026-09-17 | La precedenza è la profondità nelle regole: un livello = un non terminale | `expr` / `term` / `factor` |
| 2026-09-17 | L'associatività è la posizione del non terminale ricorsivo | `expr '+' term` contro `term '+' expr` |
| 2026-09-17 | Le parentesi sono un ciclo dal livello più basso al più alto | `factor -> '(' expr ')'` |
| 2026-09-17 | Aggiungere un operatore di pari forza = un'alternativa in più, non un livello in più | `-` messo con `+`, `/` con `*`, dedotto da `1 - 2 * 3 = -5` |
| 2026-09-17 | L'associatività a sinistra è gratis con la ricorsione a sinistra | derivato `8 / 4 / 2`, confrontati i due alberi: 1 contro 4 |
| 2026-09-17 | Derivazione dall'alto (primo nodo = radice) contro valutazione dal basso (radice = ultima operazione) | il primo `/` derivato è quello più a destra nel testo |
| 2026-09-18 | L'allineamento di un aggregato si deduce ricorsivamente fino agli **scalari**, mai dalla `sizeof`. Un array non è mai più allineato del suo elemento | `char[16]`: `sizeof` 16, `_Alignof` 1 |
| 2026-09-18 | Il non terminale dentro le parentesi decide quanto in alto si risale: puntare a `term` amputa `+` e `-` | derivato `(4*5)` con la grammatica mutilata, persa `(1+2)` |
| 2026-09-18 | L'operatore non è un nodo dell'albero: è il **tag** del nodo binario | `1 + 2` ha tre nodi, nessuno dei quali è il `+` |
| 2026-09-18 | Un prefisso in cima a `expr` ha la precedenza più bassa di tutte e si mangia ciò che ha a destra | `-2 + 3`: 1 contro -5 |
| 2026-09-18 | Due posizioni nella gerarchia distinguono solo se fra loro vive un operatore *intrappolabile* | dimostrata l'equivalenza di `-` in `factor` e di un livello `unary` |
| 2026-09-18 | Dimostrare che un controesempio **non può esistere**, invece di cercarlo | B ≡ C, argomento per assenza di livelli intermedi |
| 2026-09-18 | La forma della ricorsione è la forma della funzione: sinistra → ciclo, destra → autochiamata, nessuna → dispatch | i quattro livelli della grammatica di mango |
| 2026-09-21 | L'albero di derivazione è **invariante rispetto all'ordine di espansione**: l'ordine è una proprietà del procedimento, non della grammatica | derivato `1 + 2` a zigzag e poi sempre-a-sinistra, stesso albero |
| 2026-09-21 | Derivazione canonica sinistra e destra, e perché un parser a discesa ricorsiva è una derivazione sinistra costruita al contrario (**LL**) | dedotto dall'ordine in cui la funzione consuma i token |
| 2026-09-21 | Albero di derivazione ≠ AST: i non terminali sono impalcatura che sparisce, la precedenza resta nella forma | `(1+2)*3`: 21 nodi contro 5 |
| 2026-09-21 | Una ricorsione a destra termina perché **consuma prima di richiamarsi**: l'input residuo si accorcia strettamente a ogni giro | tracciato `- - 5` su `unary -> '-' unary` |
| 2026-09-21 | Un array è allineato come il suo elemento, non come la sua dimensione | `char[16]`: 16/1, detto senza suggerimenti al quarto giro |
| 2026-09-21 | Gli allineamenti sono sempre potenze di 2, perché il test è una AND di maschera (`p & 7 == 0`); per questo lcm e max coincidono | ricostruito dal controesempio `union {double; char[16];}` |
| 2026-09-21 | Operando conteso: la precedenza è la regola che decide chi se lo prende quando due operatori se lo contendono | `2 + 3 * 4`, il 3 conteso fra `+` e `*`: 14 contro 20 |
| 2026-09-21 | Livello = insieme di operatori che legano uguale; la precedenza decide **fra** livelli, l'associatività **dentro** un livello | `10 - 4 + 3`: la precedenza tace, 9 contro 3 |
| 2026-09-21 | Più in basso nei livelli = lega più forte = si esegue prima = più in basso nell'albero | tua domanda, non mia |
| 2026-09-21 | Una stringa dimostra una collocazione solo se le due ipotesi danno **due numeri diversi** | `^` collocato con `2 * 3 ^ 2` = 18 contro 36; `2 + 3 ^ 2` = 11 è muta |
| 2026-09-21 | Dove può stare un numero può stare un'operazione intera: è l'annidamento, e la radice della ricorsione nella grammatica | scritto da te `2 + (3*4)` sostituendo il risultato con l'operazione |
| 2026-09-21 | Il parser rende **esplicita** una struttura che nel testo è implicita | le parentesi messe da te dove avevi detto che non c'erano |
| 2026-09-21 | Le parentesi non sono un nodo dell'albero: sono un'istruzione, e spariscono | primo disegno con un pallino di troppo, proprio lì |
| 2026-09-21 | **Livello ≠ profondità.** Il livello è dell'operatore e non cambia mai; la profondità è dell'albero e cambia a ogni espressione | `3*4+2` contro `(3+4)*2`: il `+` una volta sopra e una sotto |
| 2026-09-21 | Un livello è una **catena**, e un numero è una catena da un anello solo: per questo non serve un caso a parte | contati 3 anelli in `2 + (3*4) - 5`, non 4 numeri |
| 2026-09-21 | La BNF è solo notazione: le quattro righe di `grammar.md` sono quattro frasi di italiano | scritte da te e affiancate alle regole |
| 2026-09-21 | L'associatività a sinistra è una **conseguenza** di dove sta il non terminale nella riga, non una regola a parte | `expr -> expr '+' term`: la catena cresce a sinistra |
| 2026-09-22 | Una riga di grammatica è ricorsiva quando il nome a sinistra della freccia **ricompare a destra**: la regola nomina sé stessa. L'alternanza (`\|`) è un'altra cosa | test meccanico su `term` contro `factor` |
| 2026-09-22 | La radice è l'operatore **più debole** dell'espressione; a parità di livello, il più a destra | `2 * 3 * 4` contro `2 + 3 * 4`, generalizzazione tua riparata da te |
| 2026-09-22 | Una stringa distingue due ipotesi solo se **contiene gli operatori di cui le ipotesi parlano** | `2 + 3 ^ 2` muta su `^` contro `*`; serve un `*` dentro |
| 2026-09-22 | I due mestieri del lexer: **scartare** ciò che nessuna regola produce, **raggruppare** i caratteri in simboli della grammatica | `12 + 345`: 8 caratteri, 3 simboli |
| 2026-09-22 | **Maximal munch**: la condizione è positiva sulla classe di caratteri, non un elenco di terminatori — così il lexer non deve conoscere il resto del linguaggio, e l'EOF non è un caso a parte | «finché non trova uno spazio» rotta da `12+345`, poi «finché è una cifra» |
| 2026-09-22 | Il token è **tag + valore**, e il tag non è un'etichetta: è il **permesso di leggere** l'altro campo | il `+` non ha valore, il `12` sì |
| 2026-09-22 | Il lexer **non conosce la grammatica e non ha memoria del contesto**: unario contro binario è posizionale, quindi è del parser | stesso byte `0x2D` in `5 - 3` e `- 5`; la regola «se prima non c'è una cifra» cade su `(1 + 2) - 3` |
| 2026-09-22 | **Sentinella**: l'EOF è un token come gli altri, così il caso speciale non esiste invece di essere gestito bene | design `NULL` = un controllo sul puntatore in ogni punto del parser, uno dimenticato = segfault |
| 2026-09-22 | Il carattere illegale diventa un **token di errore** con tag suo: stessa mossa dell'EOF, il caso limite entra nel tipo normale | proposto da te su `12 @ 34` |
| 2026-09-22 | Riga e colonna **non sono gratis**: sono stato mantenuto durante la scansione (+1 sul `\n`, colonna azzerata). Gratis è solo l'offset, che è il puntatore | buffer disegnato, puntatore sul `@` all'indice 7 |
| 2026-09-22 | Il lexer **non possiede il ciclo**: non può «fermarsi», risponde a una chiamata. Fermarsi al primo errore è politica del chiamante. Contratto: a ogni chiamata un token, sempre | `gcc` mostra 3 errori su 3; «lo richiamo io quando voglio» |
| 2026-09-22 | La posizione sta in **ogni** token, non solo in quello d'errore: serve al parser per riportare un errore su un token incontrato molto prima | «hai aperto questa `(` e non l'hai chiusa» |

---

## Roadmap

### Traccia attiva — **mango**, in `code/c/lang/`
- [x] Tappa 0 — Cos'è un linguaggio: token, grammatica, albero. Nessun codice
      *(chiusa il 21 set 2026 in 4 sessioni. `grammar.md` completo: quattro
      livelli annotati, tre decisioni motivate, due derivazioni con albero e
      AST. **Scritto da me su tua richiesta**, non da te — vedi il registro)*
- [x] **Tappa 0-bis — il vocabolario, da zero.** *(sessione 1
      fatta il 21 set: operando, precedenza, «legare più forte», livello,
      associatività — tutti costruiti da `2 + 3 * 4` e da `10 - 4 + 3`.
      Restano da costruire con lo stesso metodo: **grammatica** e **albero**,
      partendo dalla tabella dei livelli — **fatto nella stessa sessione**:
      annidamento, albero, livello contro profondità, la catena, e le quattro
      frasi di italiano riconosciute come le quattro righe BNF di
      `grammar.md`.)* **Chiusa il 22 set 2026**, col richiamo a freddo sulle
      quattro domande rimaste aperte. Metodo che ha funzionato:
      una domanda alla volta, il nome **dopo** averlo visto, nessuna domanda
      comparativa, e la consegna riscritta più piccola appena hai detto "non
      ho capito"
- [ ] Tappa 1 — Lexer a mano: tagged union, buffer, EOF e stati d'errore.
      **← in corso.** Sessione 1 (22 set): teoria, zero codice — due mestieri,
      maximal munch, token = **tag + valore + posizione**, il meno unario non
      è un tag, EOF e carattere illegale come token, riga/colonna come stato
      mantenuto, e il contratto «a ogni chiamata un token, sempre».
      Inventario a nove tag (otto + errore). **Prossimo passo: il `README.md` di
      tappa con specifica, contratti, `Makefile` e test — poi il `.c` lo apri
      vuoto tu**, un pezzo per sessione
- [ ] Tappa 2 — Parser a discesa ricorsiva: precedenza, e **chi possiede i nodi dell'AST** (arriva l'arena)
- [ ] Tappa 3 — Interprete ad albero: valori come tagged union, environment, scope
- [ ] Tappa 4 — Bytecode: encoding, constant pool, backpatching dei salti
- [ ] Tappa 5 — VM a stack: disciplina dello stack, dispatch loop
- [ ] Tappa 6 — Hash table tua: open addressing, FNV-1a, load factor, rehash
- [ ] Tappa 7 — Heap degli oggetti: stringhe, interning, confine valore/oggetto
- [ ] Tappa 8 — GC mark & sweep: root set, raggiungibilità
- [ ] Tappa 9 — Misurare e ottimizzare: harness di timing scritto da te

### Tracce parcheggiate
- **Audio** — `~/Work/audio`, fermo dal 2026-09-04, al punto: il passo
  successivo (inviluppo, altre forme d'onda, stereo) richiede di estrarre
  `wav.h` invece di copiare il writer una terza volta. Ripartire da:
  l'estrazione di `wav.h` + il bug UB in `sine.c`. Buon candidato per una
  deviazione di una sessione.
- **Mini-Redis** — piano del 2026-09-16 sul tuo `poll_server.c`, 9 tappe.
  Parcheggiato in `~/.claude/plans/`. Ripartire da: i difetti in tabella qui
  sopra.
- **ML** — rete neurale da zero, stile Karpathy. Si attiva dopo le tappe 6 e 9.

---

## Registro sessioni

Una riga per sessione. Serve a vedere il ritmo vero, non quello dichiarato.

| Data | Tappa | Cosa è successo |
|---|---|---|
| 2026-09-17 | — | Sessione di metodo: definito come si lavora, scelto il progetto linguaggio, ricostruito questo registro da zero |
| 2026-09-17 | 0 | Catena di rappresentazioni, AST, associatività. Il tipo del nodo porta a `union` e tagged union: misurati 40 → 24 byte con `sizeof`/`offsetof` |
| 2026-09-17 | 0 | Grammatica. BNF, ambiguità dimostrata coi due valori 9 e 7, stratificazione `expr/term/factor` derivata a fatica, poi `-` e `/` e la verifica di associatività su `8/4/2`. Sessione lunga e in salita: la BNF era stata introdotta male, in una riga, ed è stata rifatta a metà strada. Fermato prima di `grammatica.md` |
| 2026-09-18 | 0 | Chiuse le due decisioni aperte: niente `addop` (sarebbe un non terminale senza nodo), meno unario in un livello suo. Dimostrata per la prima volta un'**equivalenza** fra due grammatiche invece di una differenza. Richiamo a freddo 2/3, entrambi gli errori di rigore. Sessione finita stanca: `grammar.md` iniziato e fermo alle quattro regole |
| 2026-09-21 | 0 | Richiamo a freddo diventato lezione: ordine di espansione e invarianza dell'albero (tua domanda, tua risposta), albero di derivazione vs AST, la terza forma di ricorsione, allineamento al quarto giro. Poi `grammar.md`: consegna in quattro punti, due richieste di scriverlo io, opt-out pieno alla seconda. 459 righe scritte da me. Tappa 0 chiusa |
| 2026-09-21 | 0-bis | Ripartiti dal vocabolario dopo la tua diagnosi. Da `2 + 3 * 4`: operando conteso, precedenza, «legare più forte», livello, associatività — ognuno costruito da te e nominato dopo. Tua la domanda «più in basso = lega più forte?», che è il cardine. Chiuso collocando `^`. Sette domande, una alla volta, zero comparazioni |
| 2026-09-21 | 0-bis | Seconda metà: annidamento (`2 + (3*4)` scritto da te), l'albero disegnato a mano, **livello ≠ profondità** su `3*4+2` contro `(3+4)*2`, la catena e i suoi anelli, e infine le quattro frasi di italiano riconosciute come le quattro righe di `grammar.md`. Sedici domande in tutto, una alla volta. Il file che al mattino non sapevi leggere, alla sera l'hai riscritto |
| 2026-09-21 | 0-bis | Verifica finale a 6 domande: 4 piene (precedenza, albero di `-(2+3)*4`, lettura BNF, scelta della stringa che dimostra), 2 errori — parentesi contate come nodi su `((((5))))`, e direzione di crescita della catena. Quattro domande lasciate aperte: sono il richiamo a freddo di domani |
| 2026-09-22 | 0-bis | Richiamo a freddo sulle quattro domande aperte: 2 piene, 2 con la forma giusta e la sostanza sbagliata (criterio di ricorsione = alternanza; ipotesi sbagliate nel test di mutezza). In mezzo una generalizzazione tua — «la radice è sempre l'operatore più a destra» — falsa, e **riparata da te** in due passi: la radice è il più **debole**. Tappa 0-bis chiusa |
| 2026-09-22 | 1 | Teoria del lexer, zero codice. 8 caratteri contro 3 simboli → scartare e raggruppare. Maximal munch in tre giri, con la formulazione positiva trovata da te. Token = tag + valore, il tag come permesso di lettura. **Il meno unario non è un tag**: `(1 + 2) - 3` uccide la regola del carattere precedente, la distinzione è posizionale e sta già in due righe della grammatica. EOF come sentinella contro il design `NULL`. Inventario finale a otto tag ricomposto da te |
| 2026-09-22 | 1 | Coda: il carattere illegale risolto in anticipo. Token di errore con tag suo (tua proposta), messaggio d'errore rifatto due volte — vocabolario (carattere, non token) e contenuto (file/riga/colonna). Smontato «riga e colonna le so già»: gratis è solo l'offset, il resto è contabilità durante la scansione. Deciso che la posizione sta in **ogni** token, con la motivazione giusta — la `(` non chiusa venti token prima |
| 2026-09-22 | 1 | Coda della coda: fermarsi o no al primo errore. «Si ferma il lexer» caduto due volte — `gcc` mostra tutti gli errori, e soprattutto il lexer **non possiede il ciclo**, quindi non può fermarsi: è politica del chiamante. Terzo confine lexer/parser della stessa sessione |
