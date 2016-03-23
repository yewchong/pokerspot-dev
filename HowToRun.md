To run the whole thing:
- run loadbalancer
- run loungeserver
- run clientlounge (1 instance for every player)


For debugging purposes you can also just start a single tableserver and start as many holdem.exe instances as you want players. the port you have to connect to is shown in the server console. When a player has no chips start mysql-client and execute:
-"USE poker"
-"UPDATE pokeruser set chips=1000 where username ='myusername'"