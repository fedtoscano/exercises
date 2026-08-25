# TCP echo server (in C, da zero)

Progetto di apprendimento sulla programmazione di rete a basso livello con
l'API socket POSIX. Tre fasi, dalla più semplice alla concorrenza.

## Il ciclo di vita di un server TCP

```
  socket()      crea l'endpoint (un file descriptor)
     |
  setsockopt()  SO_REUSEADDR: riavvii senza "Address already in use"
     |
  bind()        associa il socket a una porta locale (es. :8080)
     |
  listen()      il socket diventa "passivo": accetta connessioni
     |
  accept()  <---+   BLOCCA finché un client si connette; ritorna un fd NUOVO
     |          |   (il listen fd resta per accettare gli altri)
  recv()/send() |   scambi byte col client su quel fd
     |          |
  close()  -----+   chiudi il fd del client, torna ad accept()
```

## Fasi

- **Fase 1 — echo, un client alla volta** (sei qui). Riempi i `// TODO` in
  `tcp_server.c`.
- **Fase 2 — client sequenziali robusti.** Gestione errori POSIX completa
  (`EINTR`, peer che spariscono) senza crash.
- **Fase 3 — client concorrenti con `poll()`.** Un solo processo che serve N
  client insieme via I/O multiplexing (il modello di nginx/redis).

## Comandi

```sh
make        # compila con -Wall -Wextra -Wpedantic + AddressSanitizer
make run    # avvia il server su :8080
make test   # avvia il server e verifica l'echo
make clean
```

Prova manuale (in un altro terminale, mentre gira `make run`):
```sh
python3 -c 'import socket; s=socket.create_connection(("127.0.0.1",8080)); \
s.sendall(b"ciao\n"); print(s.recv(100))'
```

## Trappole che il progetto ti fa toccare

- **Byte order**: `htons`/`htonl` — la rete è big-endian.
- **`send()` parziale**: può scrivere meno byte di quanti richiesti → serve un
  ciclo. Il test invia 5000 byte proprio per innescarlo.
- **`recv() == 0`** = il peer ha chiuso (EOF), non è un errore.
- **`SO_REUSEADDR`**: senza, dopo Ctrl-C la porta resta bloccata (TIME_WAIT).
