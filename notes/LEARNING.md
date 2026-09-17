# Percorso

Registro di studio. Lo leggo all'inizio di ogni sessione e lo aggiorno alla
fine, prima del commit. È scritto per te, non per me.

**Traccia attiva:** **mango** — un linguaggio: interprete → VM bytecode → GC — Tappa 0: cos'è un linguaggio
**Ultima sessione:** 2026-09-17 — Tappa 0, sessione 1
**Sessioni fatte su questa tappa:** 1

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

**Prossimo passo**
Chiudere la Tappa 0 con la **grammatica**: dove vive, esattamente, la regola
per cui `*` lega più forte di `+`. Non nel lexer, non nell'interprete — e
capire dove significa capire come il parser la applicherà.

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

---

## Roadmap

### Traccia attiva — **mango**, in `code/c/lang/`
- [ ] Tappa 0 — Cos'è un linguaggio: token, grammatica, albero. Nessun codice
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
