#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

/*
 * Zadanie 1 (25%)
 * a ) Napisz program wypisujący w pętli nieskończonej aktualną godzinę Po odebraniu sygnału SIGTSTP (CTRL+Z) program
 * zatrzymuje się, wypisując komunikat "Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu". Po ponownym
 * wysłaniu SIGTSTP program powraca do pierwotnego wypisywania. Program powinien również obsługiwać sygnał SIGINT.
 * Po jego odebraniu program wypisuje komunikat "Odebrano sygnał SIGINT" i kończy działanie. W kodzie programu, do
 * przechwycenia sygnałów użyj zarówno funkcji signal, jak i sigaction (np. SIGINT odbierz za pomocą signal, a SIGTSTP
 * za pomocą sigaction).
 *
 * b ) Zrealizuj powyższe zadanie, tworząc program potomny, który będzie wywoływał jedną z funkcji z rodziny exec skrypt
 * shellowy zawierający zapętlone systemowe polecenie date. Proces macierzysty będzie przychwytywał powyższe sygnały
 * i przekazywał je do procesu potomnego, tj po otrzymaniu SIGTSTP kończy proces potomka,
 * a jeśli ten został wcześniej zakończony, tworzy nowy, wznawiając działanie skryptu, a po otrzymaniu SIGINT kończy
 * działanie potomka (jeśli ten jeszcze pracuje) oraz programu.
 */

int decision_time = 0;
pid_t pid = 0;

void clrscr()
{
    system("@cls||clear");
}

void decision(int num) {
    if (decision_time == 0) {
        printf("\n Signal: %d. I am waiting for: Ctrl + z to continiue Ctrl + c to end\n", num);
    }
    decision_time = decision_time == 1 ? 0 : 1;
}

void end(int num) {
    printf("\nEnd of program - %i\n", num);
    exit(EXIT_SUCCESS);
}
void ex1(){
    time_t rawtime;
    struct tm *timeinfo;
    int i = 0;
    struct sigaction action;
    action.sa_flags = 0;
    action.sa_handler = decision;

    while (1) {
        sigaction(SIGTSTP, &action, NULL);
        signal(SIGINT, end);

        if (decision_time == 0) {
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            printf("\nCurrent local time and date: %s", asctime(timeinfo));
        }
        sleep(1);
    }
}

void ex2(){
    struct sigaction action;
    action.sa_flags = 0;
    action.sa_handler = decision;
    int child = 0;

    while (1) {
        sigaction(SIGTSTP, &action, NULL);
        signal(SIGINT, end);

        if (decision_time == 0) {
            if (child == 0) {
                child = 1;
                pid = fork();
                if (pid == 0) {
                    execl("./script.sh", "./script.sh", NULL);
                    exit(EXIT_SUCCESS);
                }
            } else {
                kill(pid, SIGKILL);
                child = 0;
            }
        }
        sleep(1);
    }
}
int main() {
    int decision;
    printf("Excersise 1 - Jakub Ziarko\n\nIf you want to turn on first point of excercise one type 1,\nif second type 2. At the end do not forget to tap \"Enter\"!\n");
    scanf("%i", &decision);

    clrscr();
    switch(decision){
        case 1 :
            ex1();
            break;
        case 2 :
            ex2();
            break;
        default:
            printf("wrong number :-(");
    }
}



