#include <thread>
#include <vector>
#include <stack>
#include <set>
#include <queue>
#include <atomic>
#include <list>

using namespace std;

const int MAX = 1000000;

double BC[MAX];//do zrobienia monitor

vector <int> neigh[MAX];
vector <int> P[MAX];

set <int> V;

void brandes(int s)
{
	stack <int> S;
	double sigma[MAX];
	int d[MAX];
	double delta[MAX];
	for (int w : V) {
		sigma[w] = 0;
		d[w] = -1;
		delta[w] = 0;
	}
	sigma[s] = 1;
	d[s] = 0;
	queue <int, list<int>> Q;
	Q.push(s);
	while (!Q.empty()) {
		int v = Q.front();
		Q.pop();
		S.push(v);
		for (int w : neigh[v]) {
			if (d[w] > 0) {
				Q.push(w);
				d[w] = d[v] + 1;
			}
			if (d[w] == d[v] + 1) {
				sigma[w] += sigma[v];
				P[w].push_back(v);
			}
		}
	}
	while (!S.empty()) {
		int w = S.top();
		S.pop();
		for (int v : P[w]) {
			delta[v] += (sigma[v] / sigma[w]) * (1 + delta[w]);
		}
		if (w != s) {
			BC[w] += delta[w];
		}
	}
}

int main(int argc, char * argv[])
{
	if (argc < 4) {
		printf("Usage: %s number_of_threads input_file_name output_file_name\n", argv[0]);
		return 1;
	}
	int threadCount = stoi(argv[1]);
	printf("otwieramy pliki: %s -> %s\n", argv[2], argv[3]);
	FILE * input = fopen(argv[2], "r");
	FILE * output = fopen(argv[3], "w");
	printf("otwarte\n");
	int v, e, cnt = 0;
	printf("rozpoczynam pobieranie danych\n");
	while (fscanf(input, "%d %d\n", &v, &e) != EOF) {
		printf(" -- pobrałem linijke %d %d\n", v, e);
		neigh[v].push_back(e);
		cnt++;
		printf("%d\n", cnt);
		if (feof(input)) {
			fclose(input);
			break;
		}
	}
	printf("%d wynikow w 0,02s", cnt);
	for (int i = 0; i < MAX; i++) {
		if (!neigh[i].empty()) {
			brandes(i);
			printf("%d\n", i);
		}
	}
	for (int i = 0; i < MAX; i++) {
		if (!neigh[i].empty())
			fprintf(output, "%d %lf\n", i, BC[i]);
	}
}
