#include <stdio.h>
#include <zconf.h>
#include <bits/types/sigset_t.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>

int max_sleep = 10;

/*
 * Napisz program, który tworzy N potomków i oczekuje na ich prośby na rozpoczęcie działania (sygnały SIGUSR1).
 * Po uzyskaniu K próśb, proces powinien pozwolić kontynuować działanie wszystkim procesom, które poprosiły o przejście
 * (wysłać im sygnał pozwolenia na rozpoczęcie pracy) i niezwłocznie akceptować każdą kolejną prośbę. Program powinien
 * wypisać listę wszystkich otrzymanych sygnałów wraz numerem PID potomka oraz kodem zakończenia procesu (w przypadku
 * otrzymania sygnału zakończenia pracy potomka).
 *
 * Program kończy działanie po zakończeniu pracy ostatniego potomka albo po otrzymaniu sygnału SIGINT (w tym wypadku
 * należy zakończyć wszystkie działające procesy potomne).
 *
 * N i M są argumentami programu.
 *
 * Zachowanie dzieci powinno być następujące: Każde dziecko najpierw symuluje pracę (usypia na 0-10 sekund). Następnie
 * wysyła sygnał SIGUSR1 prośby o przejście, a po uzyskaniu pozwolenia losuje jeden z 32 sygnałów czasu rzeczywistego
 * (SIGRTMIN,SIGRTMAX), wysyła go do rodzica i kończy działanie, zwracając liczbę sekund jaką proces wylosował do
 * uśpienia.
 *
 * Program główny powinien mieć możliwość śledzenia informacji na temat: (dla czytelności w łatwy sposób powinno dać się
 * je grupami włączyć i wyłączyć)
 *
 * tworzenia procesu potomnego (jego nr PID),
 * otrzymanych próśb od procesów potomnych,
 * wysłanych pozwoleń na wysłanie sygnału rzeczywistego
 * otrzymanych sygnałów czasu rzeczywistego(wraz z numerem sygnału)
 * zakończenia procesu potomnego (wraz ze zwróconą wartością)
 */

pid_t new_child() {
    pid_t pid = fork();
    if (pid < 0) {
        // error
        printf("Fork failed \n");
        exit(1);
    } else if (pid) {
        // parent process
        return pid;
    }
}

int kill_process(pid_t pid) {
    //printf("KILLED PROCESS %i\n", pid);
    kill(pid, SIGKILL);
}

void wake(__pid_t pid) {
    kill(pid, SIGCONT);
}

void childrend_wait(int wait) {
    printf("[%i WAIT FOR %i SECONDS]\n", getpid(), wait);
    sleep(wait);
    printf("[%i SIGSTOP]\n", getpid());
    kill(getpid(), SIGSTOP);
}

void print_childrens(pid_t *childrens, int parent_pid, int n) {
    int i;
    printf("CHILDRENS LIST OF %i PROCESS:\n", parent_pid);
    for (i = 0; i < n; i++)
        printf("%i  ", childrens[i]);
    printf("\n\n");
}

int main() {
    // declarations
    int i, n = 20;
    pid_t parent_pid = getpid();
    srand(time(NULL));

    // allocation
    int *random_times = calloc((size_t) n, sizeof(int));
    int *random_signals = calloc((size_t) n, sizeof(int));
    pid_t *childrens = calloc((size_t) n, sizeof(pid_t));

    // creating arrays which are used for random numbers
    for (i = 0; i < n; i++) {
        random_times[i] = rand() % max_sleep;
        random_signals[i] = rand() % (SIGRTMAX - SIGRTMIN) + SIGRTMIN;
    }

    for (i = 0; i < n; i++) {
        printf("%i ", random_signals[i]);
    }
    printf("\n");
    for (i = 0; i < n; i++) {
        printf("%i ", random_times[i]);
    }
    printf("\n");

    // parent informations
    printf("FIRST PROCESS PID %i\n", getpid());
    printf("FIRST PROCESS PARENT PID %i\n\n", getppid());

    // creating childrens and stoping them for random time
    for (i = 0; i < n; i++) {
        childrens[i] = new_child();
        if (childrens[i] != 0) {
            // parent
            if (i == n - 1)
                printf("CREATING CHILD BY %i\n\n", getpid());
            else
                printf("CREATING CHILD BY %i\n", getpid());
        } else {
            // child
            int c = i;
            i = n;
            childrend_wait(random_times[c]);
        }
    }

    //printf("PID MIDDLE1: %i\n", getpid());
    // printing childrens and waking them
    if (getpid() == parent_pid) {
        sleep(max_sleep + 5);
        print_childrens(childrens, parent_pid, n);
        for (i = 0; i < n; i++) {
            kill(childrens[i], SIGCONT);
        }
        printf("END: %i\n", getpid());
    }

    //printf("PID MIDDLE2: %i\n", getpid());

    if (getpid() != parent_pid) {
        printf("[%i SIGCONT]\n", getpid());
        sleep(15);
        i = 0;
        while (i < n && childrens[i] != 0)
            i++;
        kill(getppid(), random_signals[i]);
        printf("[%i: SENT SIGNAL TO PARENT %i]\n", getpid(), random_signals[i]);
        kill_process(getpid());
    } else {
        sleep(max_sleep + 5);
    }

    printf("PID MIDDLE3: %i\n", getpid());
    int status;
    pid_t child_pid, wpid;
    i = 0;

    sleep(25);
    printf("END OF PARENT\n");
    kill_process(getpid());

    return 0;
}
