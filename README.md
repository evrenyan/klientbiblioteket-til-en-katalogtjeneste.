# klientbiblioteket-til-en-katalogtjeneste.
************************* d1_udp ************************************

Funksjoner
- d1_create_client(): Oppretter en ny klient-peers-struktur.
- d1_delete(): Sletter en peers-struktur og lukker dens socket-tilkobling.
- d1_get_peer_info(): Løser og setter peers-adresseinformasjon fra et vertsnavn og serverport.
- d1_recv_data(): Mottar data fra en tilkoblet peer.
- d1_wait_ack(): Venter på en bekreftelse fra en peer.
- d1_send_data(): Sender data til en peer.
- d1_send_ack(): Sender en bekreftelsespakke (ACK) til en peer.

Implementeringen
For å bruke denne implementeringen, opprett en klient-peer-struktur ved hjelp av d1_create_client().
Deretter bruker du d1_get_peer_info() for å løse og sette peerens adresseinformasjon. 
Du kan deretter bruke d1_recv_data() for å motta data fra peeren, d1_send_data() for å sende data til peeren, og d1_send_ack() for å sende bekreftelser.

Bruk
1. Kjør Serveren: Utfør det kompilerte programmet med ønsket portnummer som argument. Dette initialiserer UDP-serveren. " ./d1_server <portnummer> "
2. Initialisering av Server: Serveren vil vise en melding som indikerer vellykket initialisering sammen med portnummeret den lytter på.
3. Venter på Forbindelser: Serveren vil begynne å lytte etter innkommende forbindelser og datapakker på den angitte porten.
4. Kjør Testklienten: Utfør det kompilerte programmet med serverens adresse og portnummer som argumenter. Dette simulerer en klientforbindelse til serveren. " ./d1_test_client <serveradresse(127.0.0.1)> <portnummer> "
5. Klientforbindelse: Testklienten vil forsøke å koble til den angitte serveradressen og porten.
6. Sender Testdata: Klienten vil sende forhåndsdefinerte kommandoer og datapakker til serveren for å teste dens funksjonalitet.

***********************  d2_lookup ******************************

1. Binær Tredatastruktur: En binær tredatastruktur er implementert ved hjelp av 'Tre'-strukturen, som har en 'id', 'verdi', 'antall_children', og et array av 'child'-pekere.
2. Rekursive Funksjoner: Rekursive funksjoner 'finn_node_i_tre', 'skrive_ut_tre', og 'slett_tre' er implementert for å skrive og manipulere binærtreet.
3. D2Client og LocalTreeStore: 'D2Client'- og 'LocalTreeStore'-strukturene er implementert for å representere en klient og en lokal trelager, henholdsvis.
4. d2_client_create og d2_client_delete: Funksjoner for å opprette og slette en 'D2Client'-instans er implementert.
5. d2_send_request og d2_recv_response: Funksjoner for å sende en forespørsel og motta en respons ved hjelp av 'D2Client'-instansen er implementert.
6. d2_alloc_local_tree og d2_free_local_tree: Funksjoner for å allokere og frigjøre en 'LocalTreeStore'-instans er implementert.
7. d2_add_to_local_tree og d2_print_tree: Funksjoner for å legge til noder i det lokale binære treet og skrive ut treet er implementert.

Hvordan Implementeringen Fungerer:

1. Funksjonen 'finn_node_i_tre' søker rekursivt etter en node med en gitt 'id' i binærtreet rotert ved 'root'.
2. Funksjonen 'skrive_ut_tre' skriver ut binærtreet rekursivt rotert ved 'root', med hver nivå innrykket med nivånummeret av bindestreker.
3. Funksjonen 'slett_tre' sletter rekursivt binærtreet rotert ved 'root', frigjør all minnebruken av nodene.
4. Funksjonen 'sett_inn_node' legger til en ny node i binærtreet lagret i 'tree', og oppretter rekursivt barnenoder etter behov.
5. Funksjonen 'd2_client_create' oppretter en ny 'D2Client'-instans, initialiserer dens peer ved hjelp av 'd1_create_client'.
6. Funksjonen 'd2_client_delete' sletter 'D2Client'-instansen, frigjør all minnebruken av peeren.
7. Funksjonen 'd2_send_request' sender en forespørsel til serveren ved hjelp av 'D2Client'-instansen, og funksjonen 'd2_recv_response' mottar responsen fra serveren.
8. Funksjonen 'd2_alloc_local_tree' allokerer en ny 'LocalTreeStore'-instans, initialiserer dens rot-node. Funksjonen 'd2_free_local_tree' frigjør 'LocalTreeStore'-instansen, sletter hele det binære treet.
9. Funksjonen 'd2_add_to_local_tree' legger til nye noder i det lokale binære treet lagret i 'nodes_out', og funksjonen 'd2_print_tree' skriver ut det lokale binære treet.

d2_lookup_mod


D2 Lookup Module tilbyr funksjonalitet for håndtering og samhandling med et trebasert oppslagssystem over et nettverk. Denne modulen muliggjør opprettelse, manipulering og overføring av tredatapstruktrurer mellom klienter og servere.

Modulen definerer følgende strukturer:

1. D2Client: Representerer en klient for det trebaserte oppslagssystemet, og inneholder en peker til en D1Peer-struktur for underliggende kommunikasjon.

2. Tre: Definerer en node i den binære tredatstrukturen, bestående av en ID, verdi, antall barn, et array av child peker, og et array av child-IDer.

3. LocalTreeStore: Representerer den lokale lagringen for det binære treet, som lagrer rotnoden og det totale antallet noder.

Bruk
1. Start Serveren: Kjør det kompilerte programmet med det ønskede portnummeret som et argument. Dette initialiserer D2-oppslags-serveren. " ./d2_server <portnummer>"
2. Initialisering av Server: Serveren vil vise en melding som indikerer vellykket initialisering sammen med portnummeret den lytter på.
3. Venter på Forespørsler: Serveren vil begynne å lytte etter innkommende forespørsler fra klienter på den angitte porten.
4. Start Testklienten: Utfør det kompilerte programmet med serverens adresse, portnummer og ID som skal slås opp som argumenter. Dette simulerer en klientforbindelse til serveren. " ./d2_test_client <serveradresse(127.0.0.1)> <portnummer> <ID> "
5. Klientinitialisering: Testklienten vil opprette en D2-oppslagsklient og sende en forespørsel til serveren for å slå opp den angitte ID-en.
6. Mottar Svar: Klienten vil motta et svar fra serveren som inneholder binære tretreenighetene knyttet til den forespurte ID-en.
7. Viser Svar: Klienten vil vise det mottatte svaret, og vise den hierarkiske strukturen til binære tretreenighetene knyttet til den forespurte ID-en.
8. Avslutter Klienten: Etter å ha mottatt svaret, vil klienten avslutte sin operasjon.
