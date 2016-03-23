#1 Setup your computer

1. Install MySQL
2. Create Database "poker"
3. Login to Mysql-console and type the following commands:
"USE mysql;"
"UPDATE user SET password = OLD\_PASSWORD('fr0gl3gs') where user='root';"
"FLUSH PRIVILEGES;"
4. stop the mysql-daemon
5. restart the deamon with "--old-passwords" as parameter. you can set it to start with that param everytime, so you dont have to restart it after rebooting

6. copy the pokerspot\_schema.txt file into your mysql/bin folder
7. open commandline and go to mysql/bin
8. enter mysql -u root -p poker < pokerspot\_schema.txt, enter password when prompted

### Now you have the database setup to work

#2 Setup the IDE/Project options
1. You will need VC6++, at least I didnt get it to work with VC2005
2. There are five projects
server/load-balancer/loadbalancer.dsw
server/lounge-server/loungeserver.dsw
server/poker-server/pokerserver.dsw
client/poker.dsw (this is the table)
client/loungedlg/lounge.dsw (this is the lounge)

Open every project and set the project path in the settings to the correct location

..compile...



