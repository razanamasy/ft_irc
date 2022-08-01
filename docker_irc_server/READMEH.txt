                     SERVEUR IRC SUR CONTENEUR DOCKER

paquets requis:
- docker-compose
- make


## Créer un conteneur docker dans lequel fonctionnera un server irc (inspircd) #


Explications:
	Le Dockerfile permet de construire l'image du conteneur (docker build)

	L'image du conteneur permet de construire le conteneur
	Le conteneur est ensuite lancé (docker run)

Les règles du Makefile:
	make start			-> démarre le conteneur
	make stop			-> stop le conteneur
	make rm_container	-> supprime le conteneur
	make rm_img			-> supprime l'image du conteneur

	make		-> construit l'image du conteneur, créer le conteneur et le lance
	make clean	-> stop le conteneur, supprime le conteneur et supprime son image

	make status	->	permet de lister les conteneurs en fonction,
					les images existantes et les conteneurs existants


############### Une fois le conteneur lancé: s'y connecter #####################


docker exec <options> <nom_du_conteneur> <commande>

=> docker exec -ti ircserv /bin/bash


################### Une fois connecté, que faire ? #############################


1.Lancer l'affichage de toutes les requêtes passant par le port 6667,
port utilisé par défaut par le serveur

	tcpdump <option> port <numéro_de_port>
	
	=> tcpdump -Aqq port 6667
	
	-A	=> essayes de convertir tous les octets des requêtes vers des caractères ASCII
	-qq	=> affiches moins d'information relatives au protocole TCP/IP

2. Utiliser irssi pour dialoguer avec le serveur
	lancer irssi
	/connect 127.0.0.1
	faire des commandes IRC (/connect, /join, /nick, /leave, /mode, ...)


##################### ANALyse d'une requête ####################################


structure:
	<timestamp> IP <envoyeur> '>' <destinataire>':' tcp <numéro_dont_on_se_fout>
	<bordel_indéchiffrable_en_ascii> <message_irc>

exemple:
-
	07:51:38.985188 IP e1r2p1.42.fr.39546 > 71cbf0514e81.ircd: tcp 16
	E..D.*@.@..d.........z.....`.7}.....X\.....
	.... .y$PING irc.local
-

///////////////////////////// analyse //////////////////////////////////////////

                    envoyeur (client)    destinataire (serveur)
                   +----------------+   +---------------+
                   |                |   |               |
07:51:38.985188 IP e1r2p1.42.fr.39546 > 71cbf0514e81.ircd: tcp 16
E..D.*@.@..d.........z.....`.7}.....X\.....
|                                         |
+-----------------------------------------+
    ça c'est du bordel dont on se fout

 ça aussi
+------+
|      |
.... .y$PING irc.local
        |            |
        +------------+
           message

////////////////////////////////////////////////////////////////////////////////


autre exemple:
-
	07:51:38.985290 IP 71cbf0514e81.ircd > e1r2p1.42.fr.39546: tcp 38
	E..Z6.@.@..............z.7}....p....Xr.....
	 .......:irc.local PONG irc.local :irc.local
-

///////////////////////////// analyse //////////////////////////////////////////

                    envoyeur (serveur)  destinataire (client)
                   +---------------+   +----------------+
                   |               |   |                |
07:51:38.985290 IP 71cbf0514e81.ircd > e1r2p1.42.fr.39546: tcp 38
E..Z6.@.@..............z.7}....p....Xr.....
|                                         |
+-----------------------------------------+
    ça c'est du bordel dont on se fout

 ça aussi
+------+
|      |
 .......:irc.local PONG irc.local :irc.local
         |                                 |
         +---------------------------------+
                     message

////////////////////////////////////////////////////////////////////////////////

pour la structure d'un message, reportez-vous à la section 2.3.1 de la RFC 1459
	https://datatracker.ietf.org/doc/html/rfc1459#section-2.3.1
