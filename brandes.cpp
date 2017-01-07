#include <cstdio>
#include <string>
#include <vector>
#include <stack>
#include <set>
#include <queue>
#include <atomic>
#include <list>
#include <thread>
#include <condition_variable>

using namespace std;

const int MAX = 10000;

double BC[MAX];//do zrobienia monitor

vector <int> neigh[MAX];

set <int> V;
queue <int, list<int>> vertQ;

int threadCount;
mutex cv_m;
condition_variable threadCV;

void brandes(int s)
{
	stack <int> S;
	vector <int> P[MAX];
	double sigma[MAX];
	int d[MAX];
	double delta[MAX];
	printf("koniec deklaracji\n");
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
		printf("operacja dla v: %d\n", v);
		for (int w : neigh[v]) {
			printf(" - sasiad numer: %d lezy na %d\n", w, d[w]);
			if (d[w] < 0) {
				printf(" * znalazlem %d\n", w);
				Q.push(w);
				d[w] = d[v] + 1;
			}
			if (d[w] == d[v] + 1) {
				printf(" ** rozszerzone %d\n", w);
				sigma[w] += sigma[v];
				P[w].push_back(v);
				printf(" ** teraz lezy na %d i ma sigme %.2lf\n", d[w], sigma[w]);
			}
		}
	}
	while (!S.empty()) {
		int w = S.top();
		S.pop();
		for (int v : P[w]) {
			printf("dla %d poprzednikiem jest %d\n i ma delte %.2lf\n", w, v, delta[w]);
			delta[v] += (sigma[v] / sigma[w]) * (1 + delta[w]);
			printf("delta %.2lf\n", delta[v]);
		}
		if (w != s) {
			printf(" # # dodaje %lf do %d\n", delta[w], w);
			{
				unique_lock<mutex> lk(cv_m);
				BC[w] += delta[w];
			}
		}
	}
}

int main(int argc, char * argv[])
{
	if (argc < 4) {
		printf("Usage: %s number_of_threads input_file_name output_file_name\n", argv[0]);
		return 1;
	}
	int maxThreads = stoi(argv[1]);
	printf("otwieramy pliki: %s -> %s\n", argv[2], argv[3]);
	FILE * input = fopen(argv[2], "r");
	FILE * output = fopen(argv[3], "w");
	printf("otwarte\n");
	int v, e, cnt = 0;
	printf("rozpoczynam pobieranie danych\n");
	while (fscanf(input, "%d %d\n", &v, &e) != EOF) {
		printf(" -- pobrałem linijke %d %d\n", v, e);
		neigh[v].push_back(e);
		V.insert(v);
		V.insert(e);
		cnt++;
		printf("%d\n", cnt);
		if (feof(input)) {
			fclose(input);
			break;
		}
	}
	printf("%d wynikow w 0,02s\n", cnt);

	for (auto i : V) {
		vertQ.push(i);
	}

	vector<thread> threads;
	for (int i = 0; i < maxThreads; i++) {
		threads.push_back(thread{
			[=] {
				while (!vertQ.empty()) {
					int s;
					{
						unique_lock<mutex> lk(cv_m);
						if (vertQ.empty())
							return;
						s = vertQ.front();
						vertQ.pop();
					}
					brandes(s);
				}
			}
		});
	}

	for (int i = 0; i < MAX; i++) {
		if (!neigh[i].empty())
			fprintf(output, "%d %.2lf\n", i, BC[i]);
	}
}
