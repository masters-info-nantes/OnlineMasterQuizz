Scenario
----------
* [Server] Mise en route, attend un premier client
* [Client] Demande connexion 
* [Server] Ajoute le client dans la liste des joueurs et renvoie l'identifiant
* [Client] Spécifie le nombre de joueurs attendus
* [Server] Assez de clients pour commencer une partie

Boucle jusqu'à la fin de la partie
* [Server] Sélectionne un client pour poser la question (les autres attendent)
* [Client] Attend une question si il n'a pas été élu
* [Client] L'élu pose une question et donne la réponse
* [Server] Reçoit la question et la propose aux clients (sauf l'élu)
* [Client] Envoi une réponse au serveur
* [Server] La première réponse reçue qui est vrai correspond au gagnant

Limitations
-------------
* Max players per game: 10
* A player can't send multiple answers
* If no player left in the game, quit
* No players can join during a game
* If the data trame doesn't match to first trame, process anyway
