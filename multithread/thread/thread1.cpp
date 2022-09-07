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
	/*thread t1(Func1);
	thread t2(Func2);
	thread t3([] {
		for (int i = 0; i < 10; i++)
		{
			cout << "Thread3 is running..." << endl;
		}
		});*/
	int result = 0;

	thread t1(Func1,&result);
	thread t2(Func2,&result);
	thread t3(Func3,&result);
	t1.join();
	t2.join();
	t3.join();
	cout << result;
	cout << "Main is Idle" << endl;
}