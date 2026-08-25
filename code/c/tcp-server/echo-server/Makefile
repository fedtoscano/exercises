CC      = gcc
CFLAGS  = -std=gnu17 -Wall -Wextra -Wpedantic -g \
          -fsanitize=address,undefined
TARGET  = server

$(TARGET): tcp_server.c
	$(CC) $(CFLAGS) -o $(TARGET) tcp_server.c

run: $(TARGET)
	./$(TARGET)

# Avvia il server, invia messaggi e verifica che l'echo torni identico.
test: $(TARGET)
	@python3 test_client.py

clean:
	rm -f $(TARGET)

.PHONY: run test clean
