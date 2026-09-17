# Percorso

Registro di studio. Lo leggo all'inizio di ogni sessione e lo aggiorno alla
fine, prima del commit. È scritto per te, non per me.

**Traccia attiva:** **mango** — un linguaggio: interprete → VM bytecode → GC — Tappa 0: cos'è un linguaggio
**Ultima sessione:** 2026-09-17 — Tappa 0, sessione 2
**Sessioni fatte su questa tappa:** 2

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

Il nucleo aritmetico è derivato e verificato, ed è nel README alla sezione 5.8:

```
expr   -> term   | expr '+' term   | expr '-' term
term   -> factor | term '*' factor | term '/' factor
factor -> NUM    | '(' expr ')'
```

Resta `grammatica.md`, che è quasi trascrizione. Due cose richiedono ancora
testa, ed è dove riparti domani:

1. `expr '+' term | expr '-' term` oppure un `addop -> '+' | '-'` usato una
   volta sola? Domanda posta e non risposta. La risposta si vede pensando alla
   Tappa 2, quando ogni non terminale diventa una funzione C.
2. **Il meno unario.** Decisione di design tua: `-2 * 3` dà `(-2) * 3` o
   `-(2 * 3)`? Serve la regola *e* la motivazione.

Poi si apre la Tappa 1, il lexer.

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
| 2026-09-17 | 3 | Dimensione e allineamento confusi: `_Alignof` dedotto dalla `sizeof` del membro più grande (union 16 → "allineata a 16") | previsione di `sizeof(Node)` col tag in coda |
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

---

## Roadmap

### Traccia attiva — **mango**, in `code/c/lang/`
- [ ] Tappa 0 — Cos'è un linguaggio: token, grammatica, albero. Nessun codice
      *(teoria chiusa; manca solo `grammatica.md`, scritta da te)*
- [ ] Tappa 1 — Lexer a mano: tagged union, buffer, EOF e stati d'errore
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
