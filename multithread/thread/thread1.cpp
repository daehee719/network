#include <iostream>
#include <thread>
using namespace std;



void Func1(int* result)
{
	for (int i = 0; i < 3333; i++)
	{

		*result += i;
	}
}
void Func2(int* result)
{
	for (int i = 3333; i < 6666; i++)
	{
		*result += i;
	}
}
void Func3(int* result)
{
	for (int i = 6666; i < 10001; i++)
	{
		*result += i;
	}
}

int main()
{
	int result = 0;
	mutex m;

	thread t1(Func1,&result, mutex);
	thread t2(Func2,&result, mutex);
	thread t3(Func3,&result, mutex);
	t1.join();
	t2.join();
	t3.join();
	cout << result;
	cout << "Main is Idle" << endl;
}