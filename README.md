# Serveur-Clients--Transfert-Fichier
Petit projet de serveur de stockage et de partage de fichier entre plusieurs client. Le but est de s'entraîner à coder en C.

Actuellement, une refonte de l'architecture est en train de se faire pour essayer d'optimiser et rendre plus facile à comprendre.

Il s'agit d'un serveur créé sur console à partir des bibliothèques de socket linux. Le but est d'avoir un serveur pouvant accueuillir
plusieurs client en même temps et de pouvoir stocker, répertorier et versionner les fichiers des clients.

Ce qui fonctionne :
- Le stockage de fichier.
- La gestion de version (chaque mise à jour du fichier, on incrémente d'un la version).
- Le listage des fichiers.
- La possibilité de changé de répertoire.
- La commande pour stopper le serveur.

Il est possble que les commandes ne marchent pas ensemble.

Ce qui manque :
- La deconnection d'un client.
- L'arrêt du serveur. (à refaire)
- Les requetes à plusieurs clients.
