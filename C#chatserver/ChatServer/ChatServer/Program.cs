using System.Net.Sockets;
using System;
using System.Net;
using System.Text;
using System.Threading;
using System.Collections; // 해쉬 쓰기 위해


namespace ConsoleApplication
{
    class Program
    {
        public static Hashtable clientList = new Hashtable();
        private static int userCount = 0;
        private Mutex mut = new Mutex();

        static void Main()
        {
            try
            {
                TcpListener serverSocket = new TcpListener(IPAddress.Any, 8888);// 소켓 만들고 바인딩
                TcpClient clientSocket = default(TcpClient);
                int counter = 0;
                byte[] bytesFrom = new byte[1024];
                string dataFromClient = "";

                serverSocket.Start();//리슨
                Console.WriteLine("C# Server Started...");
                while(true)
                {
                    counter++;
                    clientSocket = serverSocket.AcceptTcpClient();

                    counter = userCount;
                    userCount++;

                    HandleClient client = new HandleClient();
                    clientList.Add(counter, clientSocket);

                    client.StartClient(clientSocket, clientList, counter);


                }
                clientSocket.Close();
                serverSocket.Stop();
                Console.WriteLine("Exit");
                Console.ReadLine();
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }
    }
    public class HandleClient
    {
        const string COMMAND_ENTER = "#ENTER#";
        const string COMMAND_HISTORY = "#HISTORY#";

        public TcpClient clientSocket;

        public int userID;
        public string ClientID;

        private Hashtable clientList;
        private bool noConnection = false;

        public void StartClient(TcpClient inClientSocket, Hashtable cList,int userSerial) // 클라이언트 만들고 채팅하게 만듬
        {
            userID = userSerial;
            clientSocket = inClientSocket;
            clientList = cList;

            Thread ctThread = new Thread(doChat);//이 스레드는 채팅을 함
            ctThread.Start();
        }
        bool socketConnected(Socket s)// 소켓이 연결 되 있는지 확인하는 함수
        {
            bool part1 = s.Poll(1000, SelectMode.SelectRead);// selectread는 listen이 완료되었을 때 참
            bool part2 = (s.Available == 0);//데이터를 읽기 위해 대기중인지 확인. 데이터가 있으면 read 없으면 0
            if (part1 && part2)
                return false;
            else return true;
        }
    }
}