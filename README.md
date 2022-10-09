# Serveur-Clients--Transfert-Fichier
Petit projet de serveur de stockage et de partage de fichier entre plusieurs client.

Il s'agit d'un serveur créé sur console à partir des bibliothèques de socket linux. Le but est d'avoir un serveur pouvant accueuillir
plusieurs client en même temps et de pouvoir stocker, répertorier et versionner les fichiers des clients.

Ce qui fonctionne :
- Le stockage de fichier.
- La gestion de version (chaque mise à jour du fichier, on incrémente d'un la version).
- Le listage des fichiers.
- La possibilité de changé de répertoire.
- La commande pour stopper le serveur.
- Les requetes à plusieurs clients.

Il est possble que les commandes ne marchent pas ensemble.

Ce qui manque :
- La deconnection d'un client.
