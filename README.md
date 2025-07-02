# Système de recommandation client-serveur en C

## Aperçu

Ce projet est un système de recommandation implémenté en C, utilisant une architecture client-serveur. Il permet de fournir des recommandations d'items aux utilisateurs en utilisant trois algorithmes principaux : la factorisation matricielle (FM), l'algorithme des k plus proches voisins (KNN) et l'algorithme PAGERANK des GRAPHES. Le système traite des données de transactions (utilisateur, item, note, catégorie, horodatage) et utilise une communication via sockets TCP pour répondre aux requêtes des clients. Il inclut également des fonctions utilitaires pour manipuler les fichiers de données et représenter les relations entre utilisateurs et items sous forme de graphes.

Le projet est conçu pour être modulaire, avec des fichiers séparés pour les algorithmes (FM, KNN et GRAPHES), la gestion des données, et les composants client et serveur.

## Structure du projet

Le projet est organisé en plusieurs fichiers, chacun ayant un rôle spécifique :

- **main.c** : Point d'entrée principal, utilisé pour nettoyer le fichier de test (`test.txt`) en fonction du fichier d'entraînement (`train.txt`).
- **serveur.c** : Implémente le serveur qui écoute les requêtes des clients sur le port 8080, charge les données, entraîne les modèles FM et KNN, et renvoie les top-N recommandations.
- **client.c** : Implémente le client qui envoie des requêtes au serveur avec un identifiant d'utilisateur, un nombre d'items à recommander, et un algorithme (FM ou KNN).
- **function_principale.h / function_principale.c** : Fournit des fonctions utilitaires pour charger les transactions, convertir des dates en timestamps, filtrer les transactions par période, supprimer des transactions, nettoyer les fichiers de test, et générer des top-N recommandations.
- **knn.h / knn.c** : Implémente l'algorithme KNN, incluant la génération d'une matrice utilisateur-item, le calcul de la similarité de Pearson, la prédiction des notes, et l'évaluation de l'erreur (RMSE).
- **factorisation_matricielle.h / factorisation_matricielle.c** : Implémente l'algorithme de factorisation matricielle, avec l'initialisation du modèle, l'entraînement par descente de gradient, la prédiction des notes, et l'évaluation (RMSE et MAE).
- **graphers/functions.h | graphes/functions.c** : Fournit des fonctions pour représenter les données sous forme de graphes, notamment la création d'une matrice d'adjacence bipartite et la récupération des identifiants d'utilisateurs et d'items.
- **struct_transaction.h** : Définit la structure `Transaction` utilisée pour stocker les données (user_id, item_id, rating, category_id, timestamp).

## Prérequis

Pour compiler et exécuter ce projet, vous aurez besoin de :

- Un compilateur C (par exemple, `gcc`).
- Les bibliothèques standard C (`stdio.h`, `stdlib.h`, `string.h`, `math.h`, `time.h`).
- Les bibliothèques de sockets pour la communication réseau (`sys/socket.h`, `netinet/in.h`, `arpa/inet.h`, `unistd.h`).
- Un système d'exploitation compatible avec les sockets POSIX (Linux, macOS, ou sous-système Windows pour Linux).

## Compilation

Pour compiler le projet, assurez-vous que tous les fichiers source sont dans le même répertoire, avec les sous-dossiers `structures`, `knn`, et `Factorisation_matricielle`. Utilisez les commandes suivantes avec `gcc` :

### Compilation du serveur
```bash
gcc -o serveur serveur.c function_principale.c knn/knn.c Factorisation_matricielle/factorisation_matricielle.c graphes/functions.c -I. -Iknn -IFactorisation_matricielle -lm
```

### Compilation du client
```bash
gcc -o client client.c -I.
```

### Compilation du programme principal
```bash
gcc -o main main.c function_principale.c -I.
```

L'option `-lm` est nécessaire pour lier la bibliothèque mathématique. Les options `-I.` et `-Iknn -IFactorisation_matricielle` incluent les répertoires contenant les fichiers d'en-tête.

## Structure des données

Les fichiers de données (`train.txt`, `test.txt`) doivent être au format suivant, avec chaque ligne représentant une transaction :
```
user_id item_id category_id rating timestamp
```
- `user_id` : Identifiant de l'utilisateur (entier).
- `item_id` : Identifiant de l'item (entier).
- `category_id` : Identifiant de la catégorie (entier, optionnel).
- `rating` : Note donnée par l'utilisateur à l'item (flottant).
- `timestamp` : Horodatage de la transaction (entier long).

Exemple de ligne dans `train.txt` :
```
1 101 5 4.5 1625097600
```

## Utilisation

### Lancer le serveur
1. Exécutez le programme serveur :
   ```bash
   ./serveur
   ```
   Le serveur charge les données depuis `train.txt`, entraîne les modèles FM et KNN, et écoute sur le port 8080.

2. Le serveur attend des requêtes au format :
   ```
   algorithme user_id N
   ```
   - `algorithme` : "FM" ou "KNN".
   - `user_id` : Identifiant de l'utilisateur.
   - `N` : Nombre d'items à recommander.

   Exemple : `KNN 1 2` demande les 2 meilleures recommandations pour l'utilisateur 1 avec l'algorithme KNN.

3. Le serveur répond avec une liste des top-N items et leurs scores prédits.

### Lancer le client
1. Exécutez le programme client :
   ```bash
   ./client
   ```
   Le client envoie une requête au serveur (par exemple, pour l'utilisateur 1, 2 items, algorithme KNN) et affiche la réponse.

   **Note** : L'adresse IP du serveur dans `client.c` est codée en dur (`10.2.65.107`). Modifiez-la si nécessaire pour correspondre à l'adresse de votre serveur.

### Nettoyer les fichiers de test
Pour nettoyer le fichier `test.txt` en supprimant les transactions présentes dans `train.txt` :
```bash
./main
```

### Manipulation des données
- **Filtrer les transactions par période** : Utilisez la fonction `periodic_transaction` pour extraire les transactions entre deux dates (format JJ/MM/AAAA).
- **Supprimer des transactions** : Utilisez `delete_transaction` pour supprimer les transactions avec `user_id < min_U` et `item_id < min_I`.
- **Représentation en graphe** : Utilisez les fonctions de `graphes.c` pour générer une matrice d'adjacence ou récupérer les relations utilisateur-item.

## Algorithmes de recommandation

### Factorisation Matricielle (FM)
- **Description** : Décompose la matrice utilisateur-item en deux matrices de faible rang (`U` et `V`) avec des biais pour les utilisateurs et les items. Entraîné par descente de gradient.
- **Hyperparamètres** (définis dans `serveur.c`) :
  - Taux d'apprentissage (`alpha`) : 0.01
  - Régularisation (`lambda`) : 0.1
  - Nombre d'époques (`num_epochs`) : 100
  - Nombre de facteurs latents (`k`) : 10
- **Fonctions clés** :
  - `initialize_mf_model` : Initialise les matrices et biais.
  - `train_mf_model` : Entraîne le modèle.
  - `predict_mf` : Prédit une note.
  - `evaluate_mf` : Calcule RMSE et MAE.

### K-Nearest Neighbors (KNN)
- **Description** : Utilise la similarité de Pearson pour trouver les 2 voisins les plus proches (2-NN) et prédire les notes.
- **Fonctions clés** :
  - `Generate_matrix_knn` : Crée une matrice utilisateur-item à partir des transactions.
  - `Pearson` : Calcule la matrice de similarité.
  - `Predict` : Prédit une note pour un utilisateur et un item.
  - `Erreur` : Calcule l'erreur RMSE.

## Représentation des graphes
- **Matrice d'adjacence** : La fonction `get_matrice_adjascence` crée une matrice bipartite reliant utilisateurs et items.
- **Listes chaînées** : La fonction `get_data` organise les transactions en listes chaînées par `user_id` ou `item_id`.
- **Extraction des identifiants** : Les fonctions `get_userIds_and_itemIds`, `get_itemsIds_per_userId`, et `get_userIds_per_itemId` permettent d'explorer les relations dans les données.

## Limitations
- L'adresse IP du serveur est codée en dur dans `client.c`.
- Les indices négatifs dans les fichiers de données sont détectés mais peuvent causer des erreurs si non corrigés.
- La gestion de la mémoire est robuste, mais des erreurs d'allocation peuvent survenir pour de très grands ensembles de données.

## Améliorations possibles
- Ajouter la prise en charge de fichiers de configuration pour les hyperparamètres.
- Implémenter un mécanisme pour gérer dynamiquement l'adresse IP du serveur.
- Améliorer l'algorithme KNN en permettant de configurer le nombre de voisins (`k`).
- Ajouter des tests unitaires pour valider les fonctions utilitaires et les algorithmes.
- Optimiser la gestion de la mémoire pour les grands ensembles de données.

## Exemple d'utilisation
1. Créez un fichier `train.txt` avec des transactions.
3. Lancez le serveur : `./serveur`.
4. Lancez le client : `./client`.
5. Le client envoie une requête (par exemple, `KNN 1 2`) et reçoit une réponse comme :
   ```
   Top 2 items pour l'utilisateur 1 (KNN) : 101 (4.50), 102 (4.20)
   ```
# Système de recommandation client-serveur en C

## Aperçu

Ce projet est un système de recommandation implémenté en C, utilisant une architecture client-serveur. Il permet de fournir des recommandations d'items aux utilisateurs en utilisant deux algorithmes principaux : la factorisation matricielle (FM) et l'algorithme des k plus proches voisins (KNN). Le système traite des données de transactions (utilisateur, item, note, catégorie, horodatage) et utilise une communication via sockets TCP pour répondre aux requêtes des clients. Il inclut également des fonctions utilitaires pour manipuler les fichiers de données et représenter les relations entre utilisateurs et items sous forme de graphes.

Le projet est conçu pour être modulaire, avec des fichiers séparés pour les algorithmes (FM et KNN), la gestion des données, et les composants client et serveur.

## Structure du projet

Le projet est organisé en plusieurs fichiers, chacun ayant un rôle spécifique :

- **main.c** : Point d'entrée principal, utilisé pour nettoyer le fichier de test (`test.txt`) en fonction du fichier d'entraînement (`train.txt`).
- **serveur.c** : Implémente le serveur qui écoute les requêtes des clients sur le port 8080, charge les données, entraîne les modèles FM et KNN, et renvoie les top-N recommandations.
- **client.c** : Implémente le client qui envoie des requêtes au serveur avec un identifiant d'utilisateur, un nombre d'items à recommander, et un algorithme (FM ou KNN).
- **function_principale.h / function_principale.c** : Fournit des fonctions utilitaires pour charger les transactions, convertir des dates en timestamps, filtrer les transactions par période, supprimer des transactions, nettoyer les fichiers de test, et générer des top-N recommandations.
- **knn.h / knn.c** : Implémente l'algorithme KNN, incluant la génération d'une matrice utilisateur-item, le calcul de la similarité de Pearson, la prédiction des notes, et l'évaluation de l'erreur (RMSE).
- **factorisation_matricielle.h / factorisation_matricielle.c** : Implémente l'algorithme de factorisation matricielle, avec l'initialisation du modèle, l'entraînement par descente de gradient, la prédiction des notes, et l'évaluation (RMSE et MAE).
- **graphes.h / graphes.c** : Fournit des fonctions pour représenter les données sous forme de graphes, notamment la création d'une matrice d'adjacence bipartite et la récupération des identifiants d'utilisateurs et d'items.
- **struct_transaction.h** : Définit la structure `Transaction` utilisée pour stocker les données (user_id, item_id, rating, category_id, timestamp).

## Prérequis

Pour compiler et exécuter ce projet, vous aurez besoin de :

- Un compilateur C (par exemple, `gcc`).
- Les bibliothèques standard C (`stdio.h`, `stdlib.h`, `string.h`, `math.h`, `time.h`).
- Les bibliothèques de sockets pour la communication réseau (`sys/socket.h`, `netinet/in.h`, `arpa/inet.h`, `unistd.h`).
- Un système d'exploitation compatible avec les sockets POSIX (Linux, macOS, ou sous-système Windows pour Linux).

## Compilation

Pour compiler le projet, assurez-vous que tous les fichiers source sont dans le même répertoire, avec les sous-dossiers `structures`, `knn`, et `Factorisation_matricielle`. Utilisez les commandes suivantes avec `gcc` :

### Compilation du serveur
```bash
gcc -o serveur serveur.c function_principale.c knn/knn.c Factorisation_matricielle/factorisation_matricielle.c graphes.c -I. -Iknn -IFactorisation_matricielle -lm
```

### Compilation du client
```bash
gcc -o client client.c -I.
```

### Compilation du programme principal
```bash
gcc -o main main.c function_principale.c -I.
```

L'option `-lm` est nécessaire pour lier la bibliothèque mathématique. Les options `-I.` et `-Iknn -IFactorisation_matricielle` incluent les répertoires contenant les fichiers d'en-tête.

## Structure des données

Les fichiers de données (`train.txt`, `test.txt`) doivent être au format suivant, avec chaque ligne représentant une transaction :
```
user_id item_id category_id rating timestamp
```
- `user_id` : Identifiant de l'utilisateur (entier).
- `item_id` : Identifiant de l'item (entier).
- `category_id` : Identifiant de la catégorie (entier, optionnel).
- `rating` : Note donnée par l'utilisateur à l'item (flottant).
- `timestamp` : Horodatage de la transaction (entier long).

Exemple de ligne dans `train.txt` :
```
1 101 5 4.5 1625097600
```

## Utilisation

### Lancer le serveur
1. Exécutez le programme serveur :
   ```bash
   ./serveur
   ```
   Le serveur charge les données depuis `train.txt`, entraîne les modèles FM et KNN, et écoute sur le port 8080.

2. Le serveur attend des requêtes au format :
   ```
   algorithme user_id N
   ```
   - `algorithme` : "FM" ou "KNN".
   - `user_id` : Identifiant de l'utilisateur.
   - `N` : Nombre d'items à recommander.

   Exemple : `KNN 1 2` demande les 2 meilleures recommandations pour l'utilisateur 1 avec l'algorithme KNN.

3. Le serveur répond avec une liste des top-N items et leurs scores prédits.

### Lancer le client
1. Exécutez le programme client :
   ```bash
   ./client
   ```
   Le client envoie une requête au serveur (par exemple, pour l'utilisateur 1, 2 items, algorithme KNN) et affiche la réponse.

   **Note** : L'adresse IP du serveur dans `client.c` est codée en dur (`10.2.65.107`). Modifiez-la si nécessaire pour correspondre à l'adresse de votre serveur.

### Nettoyer les fichiers de test
Pour nettoyer le fichier `test.txt` en supprimant les transactions présentes dans `train.txt` :
```bash
./main
```

### Manipulation des données
- **Filtrer les transactions par période** : Utilisez la fonction `periodic_transaction` pour extraire les transactions entre deux dates (format JJ/MM/AAAA).
- **Supprimer des transactions** : Utilisez `delete_transaction` pour supprimer les transactions avec `user_id < min_U` et `item_id < min_I`.
- **Représentation en graphe** : Utilisez les fonctions de `graphes.c` pour générer une matrice d'adjacence ou récupérer les relations utilisateur-item.

## Algorithmes de recommandation

### Factorisation Matricielle (FM)
- **Description** : Décompose la matrice utilisateur-item en deux matrices de faible rang (`U` et `V`) avec des biais pour les utilisateurs et les items. Entraîné par descente de gradient.
- **Hyperparamètres** (définis dans `serveur.c`) :
  - Taux d'apprentissage (`alpha`) : 0.01
  - Régularisation (`lambda`) : 0.1
  - Nombre d'époques (`num_epochs`) : 100
  - Nombre de facteurs latents (`k`) : 10
- **Fonctions clés** :
  - `initialize_mf_model` : Initialise les matrices et biais.
  - `train_mf_model` : Entraîne le modèle.
  - `predict_mf` : Prédit une note.
  - `evaluate_mf` : Calcule RMSE et MAE.

### K-Nearest Neighbors (KNN)
- **Description** : Utilise la similarité de Pearson pour trouver les 2 voisins les plus proches (2-NN) et prédire les notes.
- **Fonctions clés** :
  - `Generate_matrix_knn` : Crée une matrice utilisateur-item à partir des transactions.
  - `Pearson` : Calcule la matrice de similarité.
  - `Predict` : Prédit une note pour un utilisateur et un item.
  - `Erreur` : Calcule l'erreur RMSE.

## Représentation des graphes
- **Matrice d'adjacence** : La fonction `get_matrice_adjascence` crée une matrice bipartite reliant utilisateurs et items.
- **Listes chaînées** : La fonction `get_data` organise les transactions en listes chaînées par `user_id` ou `item_id`.
- **Extraction des identifiants** : Les fonctions `get_userIds_and_itemIds`, `get_itemsIds_per_userId`, et `get_userIds_per_itemId` permettent d'explorer les relations dans les données.

## Limitations
- L'adresse IP du serveur est codée en dur dans `client.c`.
- Les indices négatifs dans les fichiers de données sont détectés mais peuvent causer des erreurs si non corrigés.
- La gestion de la mémoire est robuste, mais des erreurs d'allocation peuvent survenir pour de très grands ensembles de données.

## Améliorations possibles
- Ajouter la prise en charge de fichiers de configuration pour les hyperparamètres.
- Implémenter un mécanisme pour gérer dynamiquement l'adresse IP du serveur.
- Améliorer l'algorithme KNN en permettant de configurer le nombre de voisins (`k`).
- Ajouter des tests unitaires pour valider les fonctions utilitaires et les algorithmes.
- Optimiser la gestion de la mémoire pour les grands ensembles de données.

## Exemple d'utilisation
1. Créez un fichier `train.txt` avec des transactions.
3. Lancez le serveur : `./serveur`.
4. Lancez le client : `./client`.
5. Le client envoie une requête (par exemple, `KNN 1 2`) et reçoit une réponse comme :
   ```
   Top 2 items pour l'utilisateur 1 (KNN) : 101 (4.50), 102 (4.20)
   ```

## Dépendances
- Bibliothèques C standard : `stdio.h`, `stdlib.h`, `string.h`, `math.h`, `time.h`.
- Bibliothèques de sockets : `sys/socket.h`, `netinet/in.h`, `arpa/inet.h`, `unistd.h`.

## Auteurs
Ce projet a été développé dans le cadre d'un travail académique ou professionnel. Les contributeurs ne sont pas spécifiés dans les fichiers fournis.
## Dépendances
- Bibliothèques C standard : `stdio.h`, `stdlib.h`, `string.h`, `math.h`, `time.h`.
- Bibliothèques de sockets : `sys/socket.h`, `netinet/in.h`, `arpa/inet.h`, `unistd.h`.

## Auteurs
Ce projet a été développé dans le cadre d'un travail académique ou professionnel. Les contributeurs ne sont pas spécifiés dans les fichiers fournis.
