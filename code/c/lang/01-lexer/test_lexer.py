#!/usr/bin/env python3
"""Specifica eseguibile del lexer di mango.

Ogni caso: un input, l'output atteso su stdout riga per riga, e il codice di
uscita atteso. Il confronto e' esatto.

    python3 test_lexer.py            tutti i casi
    python3 test_lexer.py 2          solo la micro-tappa 2
    python3 test_lexer.py extra      solo i casi [extra]
"""

import os
import subprocess
import sys
import tempfile

LEXER = os.path.join(os.path.dirname(os.path.abspath(__file__)), "mango")

# (micro-tappa, nome, input, righe attese su stdout, exit code atteso)
CASES = [
    # --- micro-tappa 2: token a un carattere, spazi, riga/colonna ---
    ("2", "file vuoto",
     "",
     ["EOF 1:1"], 0),

    ("2", "un solo operatore",
     "+",
     ["PLUS 1:1", "EOF 1:2"], 0),

    ("2", "tutti e sei gli operatori",
     "+-*/()",
     ["PLUS 1:1", "MINUS 1:2", "STAR 1:3", "SLASH 1:4",
      "LPAREN 1:5", "RPAREN 1:6", "EOF 1:7"], 0),

    ("2", "gli spazi si scartano e spostano la colonna",
     "+   -",
     ["PLUS 1:1", "MINUS 1:5", "EOF 1:6"], 0),

    ("2", "solo spazi",
     "   \t  ",
     ["EOF 1:7"], 0),

    ("2", "il newline avanza la riga e azzera la colonna",
     "+\n*",
     ["PLUS 1:1", "STAR 2:1", "EOF 2:2"], 0),

    ("2", "newline finale: EOF sta sulla riga dopo",
     "+\n",
     ["PLUS 1:1", "EOF 2:1"], 0),

    ("2", "righe vuote in mezzo",
     "+\n\n\n-",
     ["PLUS 1:1", "MINUS 4:1", "EOF 4:2"], 0),

    ("2", "il tab conta una colonna",
     "+\t-",
     ["PLUS 1:1", "MINUS 1:3", "EOF 1:4"], 0),

    ("3", "nessun tag per il meno unario: il meno e' sempre MINUS",
     "- -5",
     ["MINUS 1:1", "MINUS 1:3", "NUM 5 1:4", "EOF 1:5"], 0),

    # --- micro-tappa 3: numeri, maximal munch ---
    ("3", "una cifra sola",
     "7",
     ["NUM 7 1:1", "EOF 1:2"], 0),

    ("3", "piu' cifre: un solo token",
     "345",
     ["NUM 345 1:1", "EOF 1:4"], 0),

    ("3", "la posizione e' quella della PRIMA cifra",
     "  345",
     ["NUM 345 1:3", "EOF 1:6"], 0),

    ("3", "otto caratteri, tre simboli",
     "12 + 345",
     ["NUM 12 1:1", "PLUS 1:4", "NUM 345 1:6", "EOF 1:9"], 0),

    ("3", "senza spazi: il numero si ferma sull'operatore",
     "12+345",
     ["NUM 12 1:1", "PLUS 1:3", "NUM 345 1:4", "EOF 1:7"], 0),

    ("3", "il numero si ferma sulla parentesi chiusa",
     "(12)",
     ["LPAREN 1:1", "NUM 12 1:2", "RPAREN 1:4", "EOF 1:5"], 0),

    ("3", "il numero si ferma a fine file",
     "12",
     ["NUM 12 1:1", "EOF 1:3"], 0),

    ("3", "zeri iniziali: si stampa il valore, non i caratteri",
     "007",
     ["NUM 7 1:1", "EOF 1:4"], 0),

    ("3", "zero",
     "0",
     ["NUM 0 1:1", "EOF 1:2"], 0),

    ("3", "espressione completa",
     "12 + (345 * 6)",
     ["NUM 12 1:1", "PLUS 1:4", "LPAREN 1:6", "NUM 345 1:7",
      "STAR 1:11", "NUM 6 1:13", "RPAREN 1:14", "EOF 1:15"], 0),

    ("3", "numeri su piu' righe",
     "12\n345\n",
     ["NUM 12 1:1", "NUM 345 2:1", "EOF 3:1"], 0),

    # --- micro-tappa 4: carattere illegale ed exit code ---
    ("4", "il carattere illegale non viene mangiato dal numero",
     "12%3",
     ["NUM 12 1:1", "ERROR '%' 1:3", "NUM 3 1:4", "EOF 1:5"], 1),

    ("4", "tre errori, non uno: la scansione prosegue",
     "@ @ @",
     ["ERROR '@' 1:1", "ERROR '@' 1:3", "ERROR '@' 1:5", "EOF 1:6"], 1),

    ("4", "errore su una riga successiva",
     "1 + 2\n3 * @\n",
     ["NUM 1 1:1", "PLUS 1:3", "NUM 2 1:5",
      "NUM 3 2:1", "STAR 2:3", "ERROR '@' 2:5", "EOF 3:1"], 1),

    ("4", "il punto decimale non esiste ancora: e' un carattere illegale",
     "1.5",
     ["NUM 1 1:1", "ERROR '.' 1:2", "NUM 5 1:3", "EOF 1:4"], 1),

    ("4", "una lettera e' illegale (le variabili arrivano alla tappa 3)",
     "2 * x",
     ["NUM 2 1:1", "STAR 1:3", "ERROR 'x' 1:5", "EOF 1:6"], 1),

    ("4", "errore a fine file",
     "1 @",
     ["NUM 1 1:1", "ERROR '@' 1:3", "EOF 1:4"], 1),

    # --- extra: overflow ---
    ("extra", "numero che non entra in un long",
     "99999999999999999999",
     ["ERROR overflow 1:1", "EOF 1:21"], 1),

    ("extra", "il piu' grande long che ci sta",
     "9223372036854775807",
     ["NUM 9223372036854775807 1:1", "EOF 1:20"], 0),
]


def run(source):
    with tempfile.NamedTemporaryFile("w", suffix=".mango", delete=False) as f:
        f.write(source)
        path = f.name
    try:
        p = subprocess.run([LEXER, path], capture_output=True, text=True)
        return p.stdout, p.returncode
    finally:
        os.unlink(path)


def main():
    if not os.path.isfile(LEXER):
        print("l'eseguibile ./mango non c'e'. Lancia `make` prima.")
        return 2

    wanted = sys.argv[1] if len(sys.argv) > 1 else None
    cases = [c for c in CASES if wanted is None or c[0] == wanted]
    if wanted is None:
        cases = [c for c in cases if c[0] != "extra"]

    passed, failed = 0, 0
    for stage, name, source, expected, code in cases:
        out, rc = run(source)
        got = out.splitlines()
        if got == expected and rc == code:
            passed += 1
            continue
        failed += 1
        print(f"\n  FALLITO  [{stage}] {name}")
        print(f"    input:   {source!r}")
        if got != expected:
            n = max(len(got), len(expected))
            for i in range(n):
                g = got[i] if i < len(got) else "<niente>"
                e = expected[i] if i < len(expected) else "<niente>"
                mark = "  " if g == e else "->"
                print(f"    {mark} atteso: {e:<28} ottenuto: {g}")
        if rc != code:
            print(f"    -> exit atteso: {code}   ottenuto: {rc}")

    print(f"\n{passed} passati, {failed} falliti")
    if wanted is None and failed == 0:
        print("(i casi [extra] non sono inclusi: `python3 test_lexer.py extra`)")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
