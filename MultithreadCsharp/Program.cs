using System;
using System.Threading;

    #region threading
class ThreadExample
{
    //public static void Func1(int num)
    //{
    //    for (int i = 0; i < num; i++)
    //    {
    //        Console.WriteLine("Thread1 is running...");
    //    }
    //}

    //public static void Func2()
    //{
    //    Thread.Sleep(1000*5);
    //    Console.WriteLine("Thread2 is running...");
    //}






    //public static void Main()
    //{

    //    Thread t1 = new Thread(() => Func1(100));
    //    t1.Start();


    //    Thread t2 = new Thread(Func2);
    //    t2.Start();
    //}
}
    #endregion
namespace work
{
    public class Program
    {
        private static Mutex mutex = new Mutex();

        static int num1 = 1, num2 = 1;

        static object locker = new object();

        public static void Main()
        {
            Program app = new Program();
            app.MultiThread();
        }

        private void MultiThread()
        {
            Thread[] muliThread =
            {
                new Thread(new ThreadStart(Th1)),
                new Thread(new ThreadStart(Th1)),
                new Thread(new ThreadStart(Th1))
            };
            foreach(Thread t in muliThread)
            {
                t.Start();
                //t.Join();
            }
        }
        private void Th1()
        {
            for (int i = 0; i < 1000; i++)
            {
                num2 = 1;
                lock(locker)
                {
                    if (num2 != 0) Console.WriteLine(num1 / num2);
                    num2 = 0;
                }
            }
        }
    }
}
