#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER 100
#define MAX_NAME 21
#define ERR 1

typedef struct proces {
	char *nume;
	int durata;
	int prioritate;
} Proces;

typedef struct NodCoada {
	Proces* val;
	struct NodCoada *next;
} NodCoada;

void show(Proces* running, FILE* fout) {

	if (running == NULL){
		fprintf(fout, "\n");
		return;
	}
	else
		fprintf(fout, "%s %d\n", running->nume, running->durata);
}
Proces* first(NodCoada *cap) {

	if (cap)
		return cap -> val;
	else
		return NULL;
}
NodCoada* enqueue(NodCoada *first_coada, Proces *p) {
/* Introduce elementul pe ultima pozitie din coada */

	NodCoada *nou, *t;

	nou = (NodCoada*) malloc(sizeof(NodCoada));
	nou -> val = p;
	nou -> next = NULL;

	if (first_coada == NULL)
		return nou;

	else {
		t = first_coada;
		while (t -> next != NULL)
			t = t -> next;
		t -> next = nou;
		return first_coada;
	}
}
NodCoada* dequeue(NodCoada *cap) {
/* Elimina primul element din coada primita ca argument */
	NodCoada *n;

	if (cap != NULL) {
		n = cap;
		cap = cap -> next;
		free(n);
	}
	return cap;
}
Proces* init_proces(char *nume, int t, int p) {

	Proces *proc;

	proc = (Proces*) malloc(sizeof(Proces));
	proc -> nume = (char*) malloc(MAX_NAME * sizeof(char));

	strcpy(proc -> nume, nume);
	proc -> durata = t;
	proc -> prioritate = p;

	return proc;
}

Proces* find_shortest_job_and_remove_from_ready(NodCoada **ready) {

	Proces *proc;
	NodCoada *n, *shortest, *prev_min, *prev;
	int durata_min;

	if ( ready == NULL || *ready == NULL)
		return NULL;

	/* gasesc valoarea minima din ready */
	shortest = *ready;
	durata_min = (*ready)->val->durata;

	n = *ready;

	while (n->next != NULL) {
		if (n->next->val->durata < durata_min) {
			durata_min = n->next->val->durata;
			shortest = n->next;
		}

		n = n->next;
	}

	if (n->val->durata < durata_min)
		shortest = n;

	durata_min = shortest->val->durata;

	/* parcurg lista inca o data, sa sterg primul nod in caz ca mai multe au val min */
	n = *ready;
	prev_min = NULL;

	if (n->val->durata == durata_min) {
		/* Nodul din capul cozii era minim */
		proc = (*ready)->val;
		n = *ready;
		*ready = (*ready)->next;
		free(n);
	}
	else {

		while (n->next != NULL) {
			if (n->next->val->durata == durata_min) {
				shortest = n->next;
				prev_min = n;
				break;
			}
			prev = n;
			n = n->next;
		}
		/* cazul in care ultimul nod are durata minima */
		if (n->val->durata == durata_min) {
			shortest = n;
			prev_min = prev;
		}

		prev_min->next = shortest->next;
		proc = shortest->val;
		free(shortest);
	}

	return proc;
}
void eliminare_nod_coada(NodCoada **ready, Proces *p) {

	NodCoada *temp, *prev, *n;

	n = *ready;
	prev = NULL;

	if (*ready == NULL || ready == NULL)
		return;

	if (strcmp(n->val->nume, p->nume) == 0) {
		/* Vreau sa sterg nodul din capul cozii */
		*ready = (*ready)->next;
		free(n);
	}
	else {
		while (n->next != NULL) {
			if (strcmp(n->next->val->nume, p->nume) == 0) {
				temp = n->next;
				n->next = temp->next;
				free(temp);
				break;
			}
			prev = n;
			n = n->next;
		}
		/* cazul in care sterg ultimul nod */
		if (strcmp(n->val->nume, p->nume) == 0) {
			prev->next = NULL;
			free(n);
		}
	}
}
Proces* first_priority(NodCoada **ready) {
/* Returneaza primul proces cu prioritatea maxima din ready*/
	Proces *proc;
	NodCoada *n, *nod_prio_max; /* n - contor */
	int prio_max;

	if (ready == NULL || *ready == NULL)
		return NULL;

	prio_max = (*ready)->val->prioritate; /* Initializam prio_max cu prioritatea primului proces din ready */
	nod_prio_max = *ready; /* Initializam nodul de prioritate maxima cu primul element din ready */
	n = *ready; /* Fixam contorul pe primul element */

/* Gasirea valorii prioritatii maxime, prio_max */
	while (n->next != NULL) {
		if (n->next->val->prioritate > prio_max) {
			prio_max = n->next->val->prioritate;
			nod_prio_max = n->next;
		}
		n = n->next;
	}

	if (n->val->prioritate > prio_max)
		nod_prio_max = n;

	prio_max = nod_prio_max->val->prioritate;
/* Sfarsitul gasirii valorii prioritatii maxime, prio_max */

	/* Parcurg lista inca o data, sa raman cu primul nod, in caz ca exista mai multe cu prio_max */
	n = *ready;
	while (n != NULL) {
		if (n->val->prioritate == prio_max) {
			proc = n->val;
			break;
		}
		n = n->next;
	}

	return proc;
}
void tick(NodCoada **ready, Proces **running, int n) {

	Proces *p;

	if (*running == NULL) {
		switch (n) {
			case 1:
				p = first(*ready);
				if (p) {
					/* p trece in stadiul de running */
					*running = p;
					*ready = dequeue(*ready);
				}
			break;
			case 2:
				p = find_shortest_job_and_remove_from_ready(ready);
				if (p) 
					*running = p;
			break;
		}
	}
	else {
		p = *running;
		p->durata--;

		if (p->durata == 0) {
			/* procesul din running este terminat, se elimina in totalitate */
			free(p->nume);
			free(p);
			*running = NULL;

			switch (n) {
			case 1:
				/* se pune procesul din ready in running */
				p = first(*ready);
				if (p) {
					*running = p;
					*ready = dequeue(*ready);
				}
			break;
			case 2:
			/* se pune in running procesul cu cea mai mica durata de viata din ready */
				p = find_shortest_job_and_remove_from_ready(ready);
				if (p)
					*running = p;
			break;
			}
		}
	}
}
void tick_RR(NodCoada **ready, Proces **running, int cuanta) {

	Proces *p;
	static int contor = 0;

	if (*running == NULL) {
		/* Trecerea primului element din ready in running, daca exista */
		p = first(*ready);
		if (p) {
			*running = p;
			contor = 0;
			*ready = dequeue(*ready);
		}
	}
	else {
		/* La fiecare trecere prin tick_RR_PP, pentru un proces din running se scade
		durata de viata si creste contorul */
		p = *running;
		p->durata--;
		contor++;
		if (p->durata == 0) {
			/* procesul este terminat */
			free(p->nume);
			free(p);
			*running = NULL;
			/* Trecere proces ready -> running */
			p = first(*ready);
			if (p) {
				contor = 0;
				*ready = dequeue(*ready);
				*running = p;
			}
		}
		else
			if (contor == cuanta) {
				/* Golire running */
				*running = NULL;
				/* Trecere proces care a stat prea mult in ready */
				*ready = enqueue(*ready, p);
				/* Trecerea primului proces din ready in running */
				p = first(*ready);
				if (p) {
					*running = p;
					contor = 0;
					*ready = dequeue(*ready);
				}
			}
	}
}

void tick_PP(NodCoada **ready, Proces **running, int cuanta) {

	Proces *p;
	static int contor = 0;

	if (*running == NULL) {
		/* Trecerea primului element din ready in running, daca exista */
		p = first_priority(ready);

		if (p) {
			*running = p;
			contor = 0;
			eliminare_nod_coada(ready, p);
		}
	}
	else {
		/* La fiecare trecere prin tick_PP, pentru un proces din running se scade
		durata de viata si creste contorul */
		p = *running;
		p->durata--;
		contor++;
		if (p->durata == 0) {
			free(p->nume);
			free(p);
			*running = NULL;
			/* trecere proces ready -> running */
			p = first_priority(ready);
			if (p) {
				contor = 0;
				*running = p;
				/* eliminare p din ready */
				eliminare_nod_coada(ready, p);
			}
		}
		else
			if (contor == cuanta) {
				/* Golire running */
				*running = NULL;
				/* Trecere proces care a stat prea mult in ready */
				*ready = enqueue(*ready, p);
				p = first_priority(ready);
				/* Trecerea primului proces din ready in running */
				if (p) {
					*running = p;
					contor = 0;
					/* eliminare p din ready */
					eliminare_nod_coada(ready, p);
				}
			}
			else {

				Proces *p_first_prio;
				p_first_prio = first_priority(ready);
				if (p_first_prio)
					if (p->prioritate < p_first_prio->prioritate) {
						/* Golire running */
						*running = NULL;
						/* Trecere proces in ready */
						*ready = enqueue(*ready, p);
						/* Trecerea procesului cu prioritate mai mare din ready in running */
						p = first_priority(ready);
						if (p) {
							*running = p;
							contor = 0;
							/* eliminare p din ready */
							eliminare_nod_coada(ready, p);
						}
					}
			}
	}
}
NodCoada *add(NodCoada *ready, char* buffer) {
/* Introduce in coada ready procesul citit in buffer */
	Proces* proc;
	char comanda, nume[100];
	int t, p;

	sscanf(buffer, "%c%s%d%d", &comanda, nume, &t, &p);

	/* creeaza procesul */
	proc = init_proces(nume, t, p);

	/* introduce procesul in coada */
	ready = enqueue(ready, proc);

	return ready;
}
NodCoada *wait(NodCoada *waiting, Proces **running) {
/* Introduce in waiting procesul din running */
	Proces* p;

    if (*running == NULL)
		return NULL;

	p = *running;
	*running = NULL;

	/* introduce procesul in coada waiting */
	waiting = enqueue(waiting, p);

	return 	waiting;
}

NodCoada* event(NodCoada **waiting, NodCoada *ready, char *buffer) {
/* Trece un proces din coada waiting in coada ready si returneaza ready */
	Proces *p;
	NodCoada *c, *prev;
	char *nume_proces_norocos;

	nume_proces_norocos = (char*) malloc(3 * sizeof(char));
	strcpy(nume_proces_norocos, buffer + 2 * sizeof(char));

	/*Cautam in waiting procesul cu numele procesului norocos */
	c = *waiting;
	prev = NULL;
	while (c != NULL) {
		if (strcmp(c->val->nume, nume_proces_norocos) == 0) {
			p = c->val;
			/* elimin nodul c din coada waiting */
			if (c == *waiting) {
				/* primul nod din coada, prev == NULL */
				*waiting = c-> next;
				free(c);
			}
            else
				if (prev != NULL) {
					prev->next = c->next;
					free(c);
				}
			/* introduce procesul in coada ready */
            ready = enqueue(ready, p);
            return ready;
		}
		prev = c;
		c = c -> next;
	}
	return ready;
}
NodCoada* multiple_add(NodCoada *ready, char *buffer) {

	const char s[2] = " "; /* Necesar pentru strtok */
	char *token, nume[MAX_NAME];
	Proces *proces;
	int i = 0, ok, t, p;
	char temp[MAX_BUFFER]; /* Necesar pentru a evita strcpy(buffer, buffer + 3),
							care creeaza probleme de memorie */

	strcpy(temp, buffer + 3);
	strcpy(buffer, temp);

	token = strtok(buffer, s);

	while (token != NULL) {
		ok = 0;
		if (i % 3 == 0)
			strcpy(nume, token);
		if (i % 3 == 1)
			t = atoi(token);
		if (i % 3 == 2) {
			p = atoi(token);
			ok = 1;
		}

		if (ok == 1) { /* Cand s-a terminat de citit un triplet */
			proces = init_proces(nume, t, p);
			/* Adaugare proces la ready */
			ready = enqueue(ready, proces);
		}
		token = strtok(NULL, s);
		i++;
	}
	return ready;
}
void FCFS_SJF(FILE *f, FILE *fout, int n) {

	char buffer[MAX_BUFFER];
	int l;
	char c[MAX_BUFFER];
	NodCoada *ready = NULL, *waiting = NULL;
	Proces *running = NULL;

	while (fgets(buffer, MAX_BUFFER, f) != NULL) {
		/* Extragerea in variablia c a comenzii din linia citita in buffer */
		l = strlen(buffer);
		if (buffer[l - 1] == '\n')
			buffer[l - 1] = '\0';

		sscanf(buffer, "%s", c);
		if (strcmp(c, "a") == 0){
			ready = add(ready, buffer);
			tick(&ready, &running, n);
		}
		if (strcmp(c, "w") == 0) {
			waiting = wait(waiting, &running);
			running = NULL;
			tick(&ready, &running, n);
		}
		if (strcmp(c, "s") == 0)
			show(running, fout);

		if (strcmp(c, "t") == 0)
			tick(&ready, &running, n);

		if (strcmp(c, "e") == 0) {
			ready = event(&waiting, ready, buffer);
			tick(&ready, &running, n);
		}
		if (strcmp(c, "ma") == 0) {
			ready = multiple_add(ready, buffer);
			tick(&ready, &running, n);
		}

	}
}
void Round_Robin(FILE *f, FILE *fout, int cuanta) {

	char buffer[MAX_BUFFER];
	int l;
	char c[MAX_BUFFER];
	NodCoada *ready = NULL, *waiting = NULL;
	Proces *running = NULL;

	while (fgets(buffer, MAX_BUFFER, f) != NULL) {
		l = strlen(buffer);

		if (buffer[l - 1] == '\n')
			buffer[l - 1] = '\0';

		sscanf(buffer, "%s", c);

		if (strcmp(c, "a") == 0){
			ready = add(ready, buffer);
			tick_RR(&ready, &running, cuanta);
		}
		if (strcmp(c, "w") == 0) {
			waiting = wait(waiting, &running);
			running = NULL;
			tick_RR(&ready, &running, cuanta);
		}
		if (strcmp(c, "s") == 0)
			show(running, fout);

		if (strcmp(c, "t") == 0)
			tick_RR(&ready, &running, cuanta);

		if (strcmp(c, "e") == 0) {
			ready = event(&waiting, ready, buffer);
			tick_RR(&ready, &running, cuanta);
		}
		if (strcmp(c, "ma") == 0) {
			ready = multiple_add(ready, buffer);
			tick_RR(&ready, &running, cuanta);
		}
	}

}

void Planificare_Prioritati(FILE *f, FILE *fout, int cuanta, int prio) {

	char buffer[MAX_BUFFER];
	int l;
	char c[MAX_BUFFER];
	NodCoada *ready = NULL, *waiting = NULL;
	Proces *running = NULL;

	while (fgets(buffer, MAX_BUFFER, f) != NULL) {
		l = strlen(buffer);

		if (buffer[l - 1] == '\n')
			buffer[l - 1] = '\0';

		sscanf(buffer, "%s", c);

		if (strcmp(c, "a") == 0){
			ready = add(ready, buffer);
			tick_PP(&ready, &running, cuanta);
		}
		if (strcmp(c, "w") == 0) {
			waiting = wait(waiting, &running);
			running = NULL;
			tick_PP(&ready, &running, cuanta);
		}
		if (strcmp(c, "s") == 0)
			show(running, fout);

		if (strcmp(c, "t") == 0)
			tick_PP(&ready, &running, cuanta);

		if (strcmp(c, "e") == 0) {
			ready = event(&waiting, ready, buffer);
			tick_PP(&ready, &running, cuanta);
		}
		if (strcmp(c, "ma") == 0) {
			ready = multiple_add(ready, buffer);
			tick_PP(&ready, &running, cuanta);
		}
	}

}
int main(int argc, char *argv[]) {

	FILE *f, *fout;
	int n;
	int x, y; /* x - cuanta, y - numar de niveluri de prioritate */

	char buffer[MAX_BUFFER];

	if (argc < 2)
        return 1;

	f = fopen(argv[1], "r");
	fout = fopen(argv[2], "w");

	if (f == NULL) {
		printf("Introduceti fisier intrare\n");
		return 1;
	}
	if (fout == NULL) {
		printf("Introduceti fisier iesire\n");
		return 1;
	}

	fgets(buffer, MAX_BUFFER, f);
	sscanf(buffer, "%d", &n);

	switch (n) {
		case 3: sscanf(buffer + 2*sizeof(char), "%d", &x); break;
		case 4:
			sscanf(buffer + 2*sizeof(char), "%d", &x);
			sscanf(buffer + 4*sizeof(char), "%d", &y);
			break;
	}

	switch (n) {
		case 1:	FCFS_SJF(f, fout, n); break;
		case 2:	FCFS_SJF(f, fout, n); break;
		case 3: Round_Robin(f, fout, x); break;
		case 4: Planificare_Prioritati(f, fout, x, y); break;
		default: printf("Nu am citit n bine\n");
	}

	fclose(fout);
	fclose(f);

	return 0;
}

