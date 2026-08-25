#!/usr/bin/env python3
"""Client di test per l'echo server: avvia ./server, invia messaggi e
verifica che tornino identici. Solo stdlib."""
import socket
import subprocess
import sys
import time

HOST, PORT = "127.0.0.1", 8080
MESSAGES = [b"hello\n", b"ciao mondo\n", b"x" * 5000]  # l'ultimo forza send parziali

def main():
    srv = subprocess.Popen(["./server"], stdout=subprocess.PIPE)
    time.sleep(0.3)  # dai tempo al server di fare bind + listen
    if srv.poll() is not None:
        print("❌ il server è uscito subito — make_listener non è ancora implementato?")
        sys.exit(1)

    ok = True
    try:
        for msg in MESSAGES:
            with socket.create_connection((HOST, PORT), timeout=2) as s:
                s.sendall(msg)
                s.shutdown(socket.SHUT_WR)  # segnala EOF al server (-> recv==0)
                echoed = b""
                while len(echoed) < len(msg):
                    chunk = s.recv(4096)
                    if not chunk:
                        break
                    echoed += chunk
            label = msg[:20] + (b"..." if len(msg) > 20 else b"")
            if echoed == msg:
                print(f"✅ echo ok  ({len(msg)} byte)  {label!r}")
            else:
                print(f"❌ atteso {len(msg)} byte, ricevuti {len(echoed)}  {label!r}")
                ok = False
    finally:
        srv.terminate()

    print("\nTUTTI I TEST PASSATI 🎉" if ok else "\nqualche test è fallito")
    sys.exit(0 if ok else 1)

if __name__ == "__main__":
    main()
