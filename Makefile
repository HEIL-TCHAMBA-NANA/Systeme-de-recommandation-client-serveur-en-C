# Variables
CC = gcc
#CFLAGS = -Wall -Wextra -std=c99 -g
INCLUDES = -I. -IFactorisation_matricielle -Iknn -Igraphes -Istructures

# Répertoires
LIB_DIR = lib
FM_DIR = Factorisation_matricielle
KNN_DIR = knn
GRAPHES_DIR = graphes
STRUCTURES_DIR = structures

# Bibliothèques
LIBS = -L$(LIB_DIR) -lfm -lknn -lgraphes -lm
# Alternative si les bibliothèques ont des noms différents:
# LIBS = -L$(LIB_DIR) -llibfm -llibknn -llibgraphes -lm

# Fichiers objets principaux
COMMON_OBJS = function_principale.o

# Cibles principales
.PHONY: all clean client serveur libs run-server run-client run-both stop-server test-knn test-fm test-local

all: client serveur

# Compilation du client
client: client.o
	$(CC) $(CFLAGS) -o client client.o

client.o: client.c
	$(CC) $(CFLAGS) $(INCLUDES) -c client.c -o client.o

# Compilation du serveur
serveur: serveur.o $(COMMON_OBJS) libs
	$(CC) $(CFLAGS) -o serveur serveur.o $(COMMON_OBJS) $(LIBS)

serveur.o: serveur.c
	$(CC) $(CFLAGS) $(INCLUDES) -c serveur.c -o serveur.o

function_principale.o: function_principale.c function_principale.h
	$(CC) $(CFLAGS) $(INCLUDES) -c function_principale.c -o function_principale.o

# Vérification des bibliothèques (optionnel)
libs:
	@echo "Vérification des bibliothèques..."
	@if [ ! -f $(LIB_DIR)/libfm.a ]; then echo "Attention: libfm.a non trouvée"; fi
	@if [ ! -f $(LIB_DIR)/libknn.a ]; then echo "Attention: libknn.a non trouvée"; fi
	@if [ ! -f $(LIB_DIR)/libgraphes.a ]; then echo "Attention: libgraphes.a non trouvée"; fi
	@echo "Vérification terminée."

# Nettoyage
clean:
	rm -f *.o client serveur server.pid *.bak

# Nettoyage complet (inclut les bibliothèques si vous voulez les reconstruire)
distclean: clean
	rm -f $(LIB_DIR)/*.a
	rm -f $(FM_DIR)/*.o
	rm -f $(KNN_DIR)/*.o
	rm -f $(GRAPHES_DIR)/*.o

# Règles pour reconstruire les bibliothèques si nécessaire
rebuild-libs: clean-libs build-libs

clean-libs:
	rm -f $(LIB_DIR)/*.a
	rm -f $(FM_DIR)/*.o
	rm -f $(KNN_DIR)/*.o
	rm -f $(GRAPHES_DIR)/*.o

build-libs:
	# Compilation Factorisation Matricielle
	$(CC) $(CFLAGS) $(INCLUDES) -c $(FM_DIR)/factorisation_matricielle.c -o $(FM_DIR)/factorisation_matricielle.o
	ar rcs $(LIB_DIR)/libfm.a $(FM_DIR)/factorisation_matricielle.o
	
	# Compilation KNN
	$(CC) $(CFLAGS) $(INCLUDES) -c $(KNN_DIR)/knn.c -o $(KNN_DIR)/knn.o
	ar rcs $(LIB_DIR)/libknn.a $(KNN_DIR)/knn.o
	
	# Compilation Graphes
	$(CC) $(CFLAGS) $(INCLUDES) -c $(GRAPHES_DIR)/functions.c -o $(GRAPHES_DIR)/functions.o
	ar rcs $(LIB_DIR)/libgraphes.a $(GRAPHES_DIR)/functions.o

# Règles pour installer/exécuter
install: all
	@echo "Compilation terminée."
	@echo "Executables: ./client et ./serveur"

# Règle pour tester la compilation
test: all
	@echo "Test de compilation réussi!"
	@echo "Client: $(shell ls -la client 2>/dev/null || echo 'Non compilé')"
	@echo "Serveur: $(shell ls -la serveur 2>/dev/null || echo 'Non compilé')"

# Commandes d'exécution
.PHONY: run-server run-client run-both stop-server

run-server: serveur
	@echo "Démarrage du serveur sur le port 8080..."
	@echo "Appuyez sur Ctrl+C pour arrêter le serveur"
	./serveur

run-client: client
	@echo "Connexion au serveur..."
	./client

run-both: all
	@echo "=== Test complet du système ==="
	@echo "Démarrage du serveur en arrière-plan..."
	@./serveur & echo $! > server.pid
	@sleep 2
	@echo "Test du client..."
	@./client
	@echo "Arrêt du serveur..."
	@if [ -f server.pid ]; then kill `cat server.pid` 2>/dev/null || true; rm -f server.pid; fi
	@echo "=== Test terminé ==="

stop-server:
	@echo "Arrêt du serveur..."
	@pkill -f "./serveur" 2>/dev/null || echo "Aucun serveur en cours d'exécution"
	@rm -f server.pid

# Test avec différents algorithmes
test-knn: all
	@echo "=== Test avec l'algorithme KNN ==="
	@echo "Modification temporaire pour KNN..."
	@sed -i.bak 's/strcpy(client.algorithm, "[^"]*")/strcpy(client.algorithm, "KNN")/' client.c 2>/dev/null || true
	@make client
	@./serveur & echo $! > server.pid
	@sleep 2
	@./client
	@if [ -f server.pid ]; then kill `cat server.pid` 2>/dev/null || true; rm -f server.pid; fi
	@if [ -f client.c.bak ]; then mv client.c.bak client.c; fi
	@echo "=== Test KNN terminé ==="

test-fm: all
	@echo "=== Test avec l'algorithme FM ==="
	@echo "Modification temporaire pour FM..."
	@sed -i.bak 's/strcpy(client.algorithm, "[^"]*")/strcpy(client.algorithm, "FM")/' client.c 2>/dev/null || true
	@make client
	@./serveur & echo $! > server.pid
	@sleep 2
	@./client
	@if [ -f server.pid ]; then kill `cat server.pid` 2>/dev/null || true; rm -f server.pid; fi
	@if [ -f client.c.bak ]; then mv client.c.bak client.c; fi
	@echo "=== Test FM terminé ==="

# Test local (change l'IP vers localhost)
test-local: all
	@echo "=== Test en local (localhost) ==="
	@sed -i.bak 's/inet_pton(AF_INET, "[^"]*"/inet_pton(AF_INET, "127.0.0.1"/' client.c
	@make client
	@./serveur & echo $! > server.pid
	@sleep 2
	@./client
	@if [ -f server.pid ]; then kill `cat server.pid` 2>/dev/null || true; rm -f server.pid; fi
	@mv client.c.bak client.c 2>/dev/null || true
	@make client
	@echo "=== Test local terminé ==="

# Règles d'aide
help:
	@echo "Makefile pour le système de recommandation client-serveur"
	@echo ""
	@echo "=== Compilation ==="
	@echo "  all          - Compile client et serveur"
	@echo "  client       - Compile uniquement le client"
	@echo "  serveur      - Compile uniquement le serveur"
	@echo "  libs         - Vérifie la présence des bibliothèques"
	@echo "  rebuild-libs - Reconstruit toutes les bibliothèques"
	@echo ""
	@echo "=== Exécution ==="
	@echo "  run-server   - Lance le serveur"
	@echo "  run-client   - Lance le client"
	@echo "  run-both     - Lance serveur puis client (test automatique)"
	@echo "  stop-server  - Arrête le serveur"
	@echo ""
	@echo "=== Tests ==="
	@echo "  test         - Test de compilation"
	@echo "  test-knn     - Test automatique avec algorithme KNN"
	@echo "  test-fm      - Test automatique avec algorithme FM"
	@echo "  test-local   - Test en local (127.0.0.1)"
	@echo ""
	@echo "=== Nettoyage ==="
	@echo "  clean        - Supprime les fichiers objets et exécutables"
	@echo "  distclean    - Nettoyage complet (inclut les bibliothèques)"
	@echo ""
	@echo "=== Aide ==="
	@echo "  help         - Affiche cette aide"
	@echo ""
	@echo "Usage: make [cible]"
	@echo ""
	@echo "Exemples:"
	@echo "  make && make run-both    # Compile et teste"
	@echo "  make run-server          # Lance le serveur"
	@echo "  make run-client          # Lance le client (dans un autre terminal)"

# Dépendances supplémentaires pour s'assurer que tout est à jour
serveur.o: function_principale.h $(FM_DIR)/factorisation_matricielle.h $(KNN_DIR)/knn.h