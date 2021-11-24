#include <iostream>
#include <random>
#include <time.h>
int main()
{
	FILE *fp = fopen("data.txt", "r");
	int n, m;
	fscanf(fp, "%d %d", &n, &m);
	std::default_random_engine e(static_cast<unsigned int>(time(nullptr)));
	std::uniform_int_distribution<> u(0, n - 1);
	int *visit = new int[n]();
	for (int i = 0; i < n; i++)
		visit[i] = 0;
	double *value = new double[n * (m + 1)]();
	int train_number = 0, test_number = 0;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j <= m; j++)
		{
			fscanf(fp, "%lf", &value[i * (m + 1) + j]);
		}
		
		int index = u(e);
		if (visit[index] == 0)
		{
			train_number = train_number + 1;
			visit[index] = 1;
		}
	}
	FILE *train = fopen("train.txt", "w");
	fprintf(train, "%d %d\n", train_number, m);
	FILE *test = fopen("test.txt", "w");
	test_number = n - train_number;
	fprintf(test, "%d %d\n", test_number, m);
	
	for (int i = 0; i < n; i++)
		if (visit[i] == 1)
		{
			for (int j = 0; j < m; j++)
			{
				fprintf(train, "%lf ", value[i * (m + 1) + j]);
			}
			fprintf(train, "%d ", (int)value[i * (m + 1) + m]);
			fprintf(train, "\n");
		}
		else
		{
			for (int j = 0; j < m; j++)
			{
				fprintf(test, "%lf ", value[i * (m + 1) + j]);
			}
			fprintf(test, "%d ", (int)value[i * (m + 1) + m]);
			fprintf(test, "\n");
		}

	return 0;
}