#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define BUFSIZE 1024
#define PORT_NO  2003
#define SERVER_ADDR "127.0.0.1"

int main(int argc, char *argv[])
{

    /************ Deklarációk ************/
    int fd, flags;

    struct sockaddr_in server;
    int server_size = sizeof server;

    struct sockaddr_in client;
    int client_size = sizeof client;

    char buffer[BUFSIZE+1];
    int bytes=BUFSIZE;				/* buffer hossza */
    char *ipstr;

    int ip,err;
    int on = 1;

    int lapok[13];  				/* kapott lapok */
    int hanyadik;  					/* játékos sorszáma */
    int i;
    char kartya[10]; 				/* választott kártya száma */
    int lap[1];
    int tmsz;


    /*********** Program  ************/

    flags = 0;

    ip = inet_addr(SERVER_ADDR);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = ip;
    server.sin_port = htons(PORT_NO);

    /* socket létrehozása */
    fd = socket(AF_INET, SOCK_STREAM, 0 );
    if ( fd < 0 ) {
        printf("%s: Socket létrehozási hiba!\n",argv[0]);
        exit(1);
    }

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on);
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof on);

    /* csatlakozás a szerverre */
    err = connect( fd, (struct sockaddr *) &server, server_size);
    if ( err < 0 ) {
        printf("%s: Nem lehet csatlakozni a szerverhez!\n",argv[0]);
        exit(2);
    }
    ipstr = inet_ntoa(server.sin_addr);
    printf("Csatlakozva a szerverre az alábbi IP-vel: %s\n", ipstr);

    /* sorszám fogadása */
    usleep(2000);
    bytes = BUFSIZE;
    err = recv( fd, buffer, bytes, flags );
    if ( err < 0 ) {
        printf("%s: Nem lehetett fogadni az adatot!\n",argv[0]);
        exit(5);
    }
    buffer[err] = 0;

    printf ("\n%s\n", buffer);

    /* Játék kezdődik */

    for(;;) {


        printf("A játék elindult!\n\n A lapok színei:\n 1-13 : treff,\t 14-26 : pikk,\n 27-39 : kőr,\t 40-52 : káró,\n A számozásuk pedig 1-13-ig sorban.\n pl 1 : kettes, 13 : Ász\n A 0 : már lerakott lap \n\nOsztás...\n");

        /* osztás */

        usleep(2000);
        bytes = BUFSIZE;
        err = recv( fd, buffer, bytes, flags );
        if ( err < 0 ) {
            printf("%s: Nem lehetett fogadni az adatot!\n",argv[0]);
            exit(5);
        }

        buffer[err] = 0;


        /*Kapott lapok tömbbe -> lapok[]*/
        sscanf (buffer,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d ", &hanyadik, &lapok[0], &lapok[1], &lapok[2], &lapok[3], &lapok[4], &lapok[5], &lapok[6], &lapok[7], &lapok[8], &lapok[9], &lapok[10], &lapok[11], &lapok[12]);



        printf("\n");

        for(;;) {			//**** egy kör ****/

            for(i=0; i<13; i++) {
                printf("%d  ", lapok[i]);
            }
            printf("\n");


            //kiir(lapok, 13);

            if(hanyadik==1) {  //**ez ha a kliens kezd***//

                //*******kártya bekérése*****//
                printf("\nHányadik kártyát akarod lerakni? ");
                scanf("%s", kartya);


                if(strcmp(kartya, "feladom")==0) {
                    bytes = sprintf(buffer, "%s" , kartya);
                }
                else {
                    bytes = sprintf(buffer, "%d", lapok[atoi(kartya)-1]);
                }


                //*******kártya elküldése****//
                usleep(2000);
                err = send( fd, buffer, bytes, flags );
                if ( err < 0 ) {
                    printf("%s: Nem lehet küldeni!\n",argv[0]);
                    exit(3);
                }
                if(strcmp(buffer, "feladom")==0) {

                    break;
                }
                else {
                    printf("Az általad választott lap: %s", buffer);
                }
                lapok[atoi(kartya)-1]=0;

                //********A másik játékos kártyájának fogadása******//
                usleep(2000);
                bytes = BUFSIZE;
                err = recv( fd, buffer, bytes, flags );
                if ( err < 0 ) {
                    printf("%s: Nem lehetett fogadni az adatot!\n",argv[0]);
                    exit(5);
                }
                buffer[err] = 0;
                //sscanf (buffer,"%d", &lap[0]);
                //kiir(lap, 1);
                printf(" A másik játékos: %s\n", buffer);
                if(strcmp(buffer, "feladom")==0) {
                    break;
                }

            }//**ez ha a kliens kezd***//


            if(hanyadik==2) { //***ez ha a kliens második****///

                //****a másik játékos lapjának fogadása és kiírása**//

                usleep(2000);
                bytes = BUFSIZE;
                err = recv( fd, buffer, bytes, flags );
                if ( err < 0 ) {
                    printf("%s: Nem lehetett fogadni az adatot!\n",argv[0]);
                    exit(5);
                }
                buffer[err] = 0;
                printf("A másik játékos: %s\n", buffer);
                if(strcmp(buffer, "feladom")==0) {
                    break;
                }


                //******kártya beolvasása és küldése*****//
                usleep(2000);
                printf("Most te jösz, írd be hányadik kártyát választod: ");
                scanf("%s", kartya);
                if(strcmp(kartya, "feladom")==0) {
                    bytes = sprintf(buffer, "%s" , kartya);
                }
                else {
                    bytes = sprintf(buffer, "%d", lapok[atoi(kartya)-1]);
                }

                //bytes = sprintf(buffer, "%d", lapok[kartya-1]);
                err = send( fd, buffer, bytes, flags );
                if ( err < 0 ) {
                    printf("%s: Nem lehet küldeni!\n",argv[0]);
                    exit(3);
                }
                if(strcmp(buffer, "feladom")==0) {
                    break;
                }
                else {
                    printf("Az általad választott lap: %s ", buffer);
                }
                lapok[atoi(kartya)-1]=0;

            }//***ez ha a kliens második****///


            //Ez mind a két esetben ugyanaz//

            //******eredmény fogadása, kiírása******//
            usleep(2000);
            bytes = BUFSIZE;
            err = recv( fd, buffer, bytes, flags );
            if ( err < 0 ) {
                printf("%s: Nem lehetett fogadni az adatot!\n",argv[0]);
                exit(5);
            }
            buffer[err] = 0;

            if(strcmp(buffer, "Vege")==0) {
                tmsz=1;
                printf("Eddigi szerzett pontok: ");
                break;
            }
            printf("Eddigi szerzett pontok: %s\n", buffer);


            //***ki kezd***
            usleep(2000);
            bytes = BUFSIZE;
            err = recv( fd, buffer, bytes, flags );
            if ( err < 0 ) {
                printf("%s: Nem lehetett fogadni az adatot!\n",argv[0]);
                exit(5);
            }
            buffer[err] = 0;
            hanyadik=atoi(buffer);


        } //for(;;) új körhöz


        usleep(2000);
        bytes = BUFSIZE;
        err = recv( fd, buffer, bytes, flags );
        if ( err < 0 ) {
            printf("%s: Nem lehetett fogadni az adatot!\n",argv[0]);
            exit(5);
        }
        buffer[err] = 0;
        printf("%s\n", buffer);

        printf("Megpróbálod mégegyszer (ujra), vagy abbahagyod(vege)?");
        scanf("%s", buffer);

        bytes=strlen(buffer);
        usleep(2000);
        err = send( fd, buffer, bytes, flags );
        if ( err < 0 ) {
            printf("%s: Nem lehet küldeni!\n",argv[0]);
            exit(3);
        }

        usleep(2000);
        bytes = BUFSIZE;
        err = recv( fd, buffer, bytes, flags );
        if ( err < 0 ) {
            printf("%s: Nem lehetett fogadni az adatot!\n",argv[0]);
            exit(5);
        }
        buffer[err] = 0;
        if(strcmp(buffer, "Vege")==0) {
            printf("A szerver leáll, a játéknak vége.\n");
            usleep(1000000);
            break;
        }

        printf("A szerver újraindul.\n");


    }//for(;;) az újrajátszáshoz

    /* socket lezárása */
    close(fd);
    /* helyes leállás */
    exit(0);
}

