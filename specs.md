Scenario
----------
* [Server] Mise en route, attend au moins X clients
* [Client] Demande connecxion avec un pseudo
* [Server] Ajoute le client dans la liste des joueurs
* [Server] Assez de clients pour commencer une partie

Boucle jusqu'à la fin de la partie
* [Server] Sélectionne un client pour poser la question (les autres attendent)
* [Client] Attend une question si il n'a pas été élu
* [Client] L'élu pose une question et donne la réponse
* [Server] Reçoit la question et la propose aux clients (sauf l'élu)
* [Client] Envoi une réponse au serveur
* [Server] La première réponse reçue qui est vrai correspond au gagnant
