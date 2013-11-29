#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFSIZE 1024
#define PORT_NO  2003


int main(int argc, char *argv[])
{

    /************ Deklarálás *************/
    int fd, fdc1, fdc2, flags;
    struct sockaddr_in server;
    int server_size = sizeof server;
    struct sockaddr_in client;
    int client_size = sizeof client;
    char buffer[BUFSIZE+1];
    int bytes=BUFSIZE;
    char *ipstr1, *ipstr2;
    int ip1,ip2,err;
    int on = 1;

    /********* Program kezdete **********/

    printf("Szerver elindult, varakozas a kliensekre.\n");
    flags = 0;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT_NO);
    /* socket létrehozása */
    fd = socket(AF_INET, SOCK_STREAM, 0 );
    if ( fd < 0 ) {
        printf("%s: Socket létrehozási hiba!\n",argv[0]);
        exit(1);
    }

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on); /* socket újrafelhasználhatósága */
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof on); /* socket életben tartása */

    /* bindelés */
    err = bind( fd, (struct sockaddr *) &server, server_size);
    if ( err < 0 ) {
        printf("%s: Bind hiba!\n",argv[0]);
        exit(2);
    }
    /* figyelés */
    err = listen( fd, 10);
    if ( err < 0 ) {
        printf("%s: Listen hiba!\n",argv[0]);
        exit(3);
    }
    /* első kliens csatlakozása */
    fdc1 = accept(fd, (struct sockaddr *) &client, &client_size);
    if ( fdc1 < 0 ) {
        printf("%s: Accept hiba\n",argv[0]);
        exit(4);
    }
    /* első kliens mentése */
    ip1 = client.sin_addr.s_addr;
    ipstr1 = inet_ntoa(server.sin_addr);
    printf("Kliens csatlakozott az alábbi IP-ről: %s\n", ipstr1);

    /* első kliensnek jelzés küldése */
    bytes = strlen("1: Csatlakoztál a szerverhez, te vagy az első játékos. Várunk a második játékos csatlakozására.\n");
    err = send( fdc1, "1: Csatlakoztál a szerverhez, te vagy az első játékos. Várunk a második játékos csatlakozására.\n", bytes, flags );
    if ( err < 0 ) {
        printf("%s: Küldés sikertelen\n",argv[0]);
        exit(3);
    }
    printf ("Elküldve:  1: Csatlakoztál a szerverhez, te vagy az első játékos. Várunk a második játékos csatlakozására.\n\n");

    /* második kliens csatlakozása */
    fdc2 = accept(fd, (struct sockaddr *) &client, &client_size);
    if ( fdc2 < 0 ) {
        printf("%s: Accept hiba\n",argv[0]);
        exit(4);
    }

    /* második kliens mentése */
    ip2 = client.sin_addr.s_addr;
    ipstr2 = inet_ntoa(server.sin_addr);
    printf("Kliens csatlakozott az alábbi IP-ről: %s\n", ipstr2);

    /* második kliensnek jelzés küldése */
    bytes = strlen("2: Csatlakoztál a szerverhez, te vagy a második játékos. A játék azonnal indul.\n");
    err = send( fdc2, "2: Csatlakoztál a szerverhez, te vagy a második játékos. A játék azonnal indul.\n", bytes, flags );
    if ( err < 0 ) {
        printf("%s: Nem lehet küldeni!\n",argv[0]);
        exit(3);
    }
    printf ("Elküldve:  2: Csatlakoztál a szerverhez, te vagy a második játékos. A játék azonnal indul.\n\n");


//*********** Játék *************\

    int i,j, k, egyedi;
    int pakli[52];
    int kevertpakli[52];
    int osztott[52];
    int fdc1Hanyadik;
    int fdc2Hanyadik;
    int fdc1Lap;
    int fdc2Lap;
    int fdc1Pontok;
    int fdc2Pontok;
    int pontok;
    int feladom;
    int tmsz; 		//igaz, ha több mint száz pontja van valakinek
    int vege=0;

//************* Új játék **************/


    for(;;) { /*újrajátszáshoz*/

        feladom=0;
        tmsz=0;
        vege=0;

        fdc1Pontok=0; 						/*****pontok nullázása***/
        fdc2Pontok=0;

        for(i=0; i<52; i++) {
            pakli[i]=i+1;   /*******pakli*******/
        }

        osztott[0]=1;
        kevertpakli[0]=1;
        for(i=1; i<52; i++) {					/*********keverés**********/
            do {
                egyedi=0;
                k=rand()%52;
                for(j=0; j<i; j++) {
                    if(osztott[j]==k) {
                        egyedi=1;
                    }
                }

            } while(egyedi);
            osztott[i]=k;
            kevertpakli[i]=pakli[k];
            //printf("%d. %d\n ", i+1, elso[i]);

        }

        for(i=0; i<13; i++) {
            if(kevertpakli[i]==1) {
                fdc1Hanyadik=1;
                fdc2Hanyadik=2;
            }
            else {
                fdc1Hanyadik=2;
                fdc2Hanyadik=1;
            }
        }
        /****** Osztás az első játékosnak ********/

        bytes = sprintf(buffer, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d ", fdc1Hanyadik, kevertpakli[0], kevertpakli[1], kevertpakli[2], kevertpakli[3], kevertpakli[4], kevertpakli[5], kevertpakli[6], kevertpakli[7], kevertpakli[8], kevertpakli[9], kevertpakli[10], kevertpakli[11], kevertpakli[12]);
        usleep(100000);
        err = send( fdc1, buffer, bytes, flags );
        if ( err < 0 ) {
            printf("%s: Küldés sikertelen\n",argv[0]);
            exit(3);
        }
        printf ("Osztás az első játékosnak.... OK.\n %s\n", buffer);


//******Osztás a második játékosnak*******//
        usleep(100000);
        bytes = sprintf(buffer, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d ", fdc2Hanyadik, kevertpakli[13], kevertpakli[14], kevertpakli[15], kevertpakli[16], kevertpakli[17], kevertpakli[18], kevertpakli[19], kevertpakli[20], kevertpakli[21], kevertpakli[22], kevertpakli[23], kevertpakli[24], kevertpakli[25]);

        err = send( fdc2, buffer, bytes, flags );
        if ( err < 0 ) {
            printf("%s: Nem lehet küldeni!\n",argv[0]);
            exit(3);
        }
        printf ("Osztás a második játékosnak.... OK.\n %s\n", buffer);

//******Az első játékos kezd?*********//

        if(fdc1Hanyadik==1) {
            printf("Az első játékos kezd\n");
        }
        if(fdc1Hanyadik==2) {
            printf("A második játékos kezd\n");
        }

        /********* Egy kör *********/
        for(;;) {
            if(fdc1Hanyadik==1) {

                /****Az első lap fogadása****/
                usleep(2000);
                bytes = BUFSIZE;
                err = recv( fdc1, buffer, bytes, flags );
                if ( err < 0 ) {
                    printf("%s: Nem lehetett fogadni az adatot!\n",argv[0]);
                    exit(5);
                }
                buffer[err] = 0;
                printf("Az első játékos: %s\n", buffer);


                /*****Az első lap továbbküldése a második kliensnek*****/
                usleep(2000);
                bytes = sprintf(buffer, "%s", buffer);
                err = send( fdc2, buffer, bytes, flags );
                if ( err < 0 ) {
                    printf("%s: Nem lehet küldeni!\n",argv[0]);
                    exit(3);
                }
                if(strcmp(buffer, "feladom")==0) {
                    feladom=1;
                    break;
                }
                fdc1Lap=atoi(buffer);


                /****A második lap fogadása****/
                usleep(2000);
                bytes = BUFSIZE;
                err = recv( fdc2, buffer, bytes, flags );
                if ( err < 0 ) {
                    printf("%s: Nem lehetett fogadni az adatot!\n",argv[0]);
                    exit(5);
                }
                buffer[err] = 0;

                printf("A második játékos: %s\n", buffer);

                /*****Az második lap továbbküldése az első kliensnek*****/
                usleep(2000);
                bytes = sprintf(buffer, "%s", buffer);
                err = send( fdc1, buffer, bytes, flags );
                if ( err < 0 ) {
                    printf("%s: Nem lehet küldeni!\n",argv[0]);
                    exit(3);
                }

                if(strcmp(buffer, "feladom")==0) {
                    feladom=2;
                    break;
                }
                fdc2Lap=atoi(buffer);


            } //fdc1 kezd


            if(fdc1Hanyadik==2) {

                /****Az első lap fogadása****/
                usleep(2000);
                bytes = BUFSIZE;
                err = recv( fdc2, buffer, bytes, flags );
                if ( err < 0 ) {
                    printf("%s: Nem lehetett fogadni az adatot!\n",argv[0]);
                    exit(5);
                }
                buffer[err] = 0;

                printf("A második játékos: %s\n", buffer);

                /*****Az első lap továbbküldése a második kliensnek*****/
                usleep(2000);
                bytes = sprintf(buffer, "%s", buffer);
                err = send( fdc1, buffer, bytes, flags );
                if ( err < 0 ) {
                    printf("%s: Nem lehet küldeni!\n",argv[0]);
                    exit(3);
                }
                if(strcmp(buffer, "feladom")==0) {
                    feladom=2;
                    break;
                }
                fdc2Lap=atoi(buffer);

                /****A második lap fogadása****/
                usleep(2000);
                bytes = BUFSIZE;
                err = recv( fdc1, buffer, bytes, flags );
                if ( err < 0 ) {
                    printf("%s: Nem lehetett fogadni az adatot!\n",argv[0]);
                    exit(5);
                }
                buffer[err] = 0;
                printf("Az első játékos: %s\n", buffer);

                /*****Az második lap továbbküldése az első kliensnek*****/
                usleep(2000);
                bytes = sprintf(buffer, "%s", buffer);
                err = send( fdc2, buffer, bytes, flags );
                if ( err < 0 ) {
                    printf("%s: Nem lehet küldeni!\n",argv[0]);
                    exit(3);
                }

                if(strcmp(buffer, "feladom")==0) {
                    feladom=1;
                    break;
                }
                fdc1Lap=atoi(buffer);

            } // ha fdc1 a második



            //***ki viszi a lapokat***//
            if(fdc1Lap/13!=fdc2Lap/13) {
                ;   //27 //24
            }
            else if(fdc1Lap>fdc2Lap) {
                fdc1Hanyadik=1;
            }
            else if(fdc1Lap<fdc2Lap) {
                fdc1Hanyadik=2;
            }

            //***a lapok mennyi pontot érnek***/
            pontok=0;
            if(fdc1Lap==24 || fdc2Lap==24) {
                pontok=pontok+13;
            }
            if(fdc1Lap>=27 && fdc1Lap<=39) {
                pontok++;
            }
            if(fdc2Lap>=27 && fdc2Lap<=39) {
                pontok++;
            }

            //printf("Az első kliens viszi a lapokat. Ebben a körben szerzett pontok száma: %d \n", pontok);

            if(fdc1Hanyadik==1) {
                printf("Az első játékos viszi a lapokat. Ebben a körben szerzett pontok száma: %d \n", pontok);
                fdc1Pontok=fdc1Pontok+pontok;
            }
            else if(fdc1Hanyadik==2) {
                fdc2Pontok=fdc2Pontok+pontok;
                printf("A második játékos viszi a lapokat. Ebben a körben szerzett pontok száma: %d \n", pontok);
            }

            printf("Az első játékos pontjai: %d\t A második játékos pontjai: %d \n", fdc1Pontok, fdc2Pontok);

            //****Eredmény küldése ****/


            //***Első játékosnak***/
            if(fdc1Pontok>=100 || fdc2Pontok >= 100) {
                bytes=sprintf(buffer, "Vege");
                tmsz=1;

            }
            else {
                bytes=sprintf(buffer, "%d", fdc1Pontok);
            }
            usleep(2000);
            err = send( fdc1, buffer, bytes, flags );
            if ( err < 0 ) {
                printf("%s: Nem lehet küldeni!\n",argv[0]);
                exit(3);
            }



            //***Második játékosnak***/
            if(fdc1Pontok>=100 || fdc2Pontok >= 100) {
                bytes=sprintf(buffer, "Vege");
                tmsz=1;

            }
            else {
                bytes=sprintf(buffer, "%d", fdc2Pontok);
            }
            usleep(2000);
            err = send( fdc2, buffer, bytes, flags );
            if ( err < 0 ) {
                printf("%s: Nem lehet küldeni!\n",argv[0]);
                exit(3);
            }
            if(tmsz) {
                break;
            }


            //******Elküldjük ki kezd*****/
            if(fdc1Hanyadik==1) {
                bytes=sprintf(buffer, "1");
            }
            else {
                bytes=sprintf(buffer, "2");
            }
            usleep(2000);
            err = send( fdc1, buffer, bytes, flags );
            if ( err < 0 ) {
                printf("%s: Nem lehet küldeni!\n",argv[0]);
                exit(3);
            }

            if(fdc1Hanyadik==1) {
                bytes=sprintf(buffer, "2");
            }
            else {
                bytes=sprintf(buffer, "1");
            }
            usleep(2000);
            err = send( fdc2, buffer, bytes, flags );
            if ( err < 0 ) {
                printf("%s: Nem lehet küldeni!\n",argv[0]);
                exit(3);
            }

        } /*újkör for*/

        //**végeredmény az első játékosnak**//
        if(feladom==1) {
            bytes=sprintf(buffer, "Vesztettél, feladtad.");
        }
        if(feladom==2) {
            bytes=sprintf(buffer, "Nyertél, az ellenfeled feladta.");
        }
        if(tmsz && fdc1Pontok<fdc2Pontok) {
            bytes=sprintf(buffer, "%d\nNyertél",fdc1Pontok);
        }
        if(tmsz && fdc1Pontok>fdc2Pontok) {
            bytes=sprintf(buffer, "%d\nVesztettél",fdc1Pontok);
        }
        if(tmsz && fdc1Pontok==fdc2Pontok) {
            bytes=sprintf(buffer, "%d\nDöntetlen",fdc1Pontok);
        }
        usleep(2000);
        err = send( fdc1, buffer, bytes, flags );
        if ( err < 0 ) {
            printf("%s: Nem lehet küldeni!\n",argv[0]);
            exit(3);
        }
        //***végeredmény a második játékosnak***/
        if(feladom==1) {
            bytes=sprintf(buffer, "Nyertél, az ellenfeled feladta.");
        }
        if(feladom==2) {
            bytes=sprintf(buffer, "Vesztettél, feladtad.");
        }
        if(tmsz && fdc1Pontok>fdc2Pontok) {
            bytes=sprintf(buffer, "%d\nNyertél",fdc2Pontok);
        }
        if(tmsz && fdc1Pontok<fdc2Pontok) {
            bytes=sprintf(buffer, "%d\nVesztettél",fdc2Pontok);
        }
        if(tmsz && fdc1Pontok==fdc2Pontok) {
            bytes=sprintf(buffer, "%d\nDöntetlen",fdc2Pontok);
        }
        usleep(2000);
        err = send( fdc2, buffer, bytes, flags );
        if ( err < 0 ) {
            printf("%s: Nem lehet küldeni!\n",argv[0]);
            exit(3);
        }


        //****Első játékos válasza****/
        usleep(2000);
        bytes = BUFSIZE;
        err = recv( fdc1, buffer, bytes, flags );
        if ( err < 0 ) {
            printf("%s: Nem lehetett fogadni az adatot!\n",argv[0]);
            exit(5);
        }
        buffer[err]=0;
        printf("Első játékos: %s\n", buffer);
        if(strcmp(buffer, "vege")==0) {
            vege++;
        }


        //***** második játékos válasza ****/
        usleep(2000);
        bytes = BUFSIZE;
        err = recv( fdc2, buffer, bytes, flags );
        if ( err < 0 ) {
            printf("%s: Nem lehetett fogadni az adatot!\n",argv[0]);
            exit(5);
        }
        buffer[err]=0;
        printf("Második játékos: %s\n", buffer);
        if(strcmp(buffer, "vege")==0) {
            vege++;
        }

        //***ha egyikük "vege"****/
        if(vege)  {
            bytes=sprintf(buffer, "Vege");
            usleep(2000);
            err = send( fdc1, buffer, bytes, flags );
            if ( err < 0 ) {
                printf("%s: Nem lehet küldeni!\n",argv[0]);
                exit(3);
            }

            bytes=sprintf(buffer, "Vege");
            usleep(2000);
            err = send( fdc2, buffer, bytes, flags );
            if ( err < 0 ) {
                printf("%s: Nem lehet küldeni!\n",argv[0]);
                exit(3);
            }

            break;
        }

        else {
            bytes=sprintf(buffer, "Ujra");
            usleep(2000);
            err = send( fdc1, buffer, bytes, flags );
            if ( err < 0 ) {
                printf("%s: Nem lehet küldeni!\n",argv[0]);
                exit(3);
            }

            bytes=sprintf(buffer, "Ujra");
            usleep(2000);
            err = send( fdc2, buffer, bytes, flags );
            if ( err < 0 ) {
                printf("%s: Nem lehet küldeni!\n",argv[0]);
                exit(3);
            }
            usleep(2000*1000);

        }


    }/*újrajátszás for*/


    /* kapcsolatok zárása */
    close(fd);
    close(fdc1);
    close(fdc2);
    /* helyes leállás */
    exit(0);
}


