#include <mutex>
#include <iostream>
#include <thread>

using namespace std;

void func1(int *a, int min, int max, mutex* m1, mutex* m2)
{
	for (int i = min; i <= max; i++)
	{
		m1->lock();
		//bool isLocked = m2->try_lock();
		m2->lock();
		cout << "쓰레드 1 : "<<i << endl;
		m2->unlock();
		m1->unlock();
	}
}

void func2(int* a, int min, int max, mutex* m1, mutex* m2)
{
	for (int i = min; i <= max; i++)
	{
		while (true)
		{
			m2->lock();
			bool isLocked = m1->try_lock();
			if (isLocked == false)
			{
				continue;
			}
			else
			{
				cout << "쓰레드 2 : " << i << endl;

				m1->unlock();
				m2->unlock();
				break;
			}
		}
	}
}

int main()
{

	mutex m1, m2;

	int a = 0;

	thread t1(func1, &a, 0, 10, &m1, &m2);
	thread t2(func2, &a,10, 20, &m1, &m2);
	t1.join();
	t2.join();

}