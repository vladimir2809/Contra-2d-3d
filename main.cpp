#include <SFML/Graphics.hpp>
#include <time.h>
#include <stdio.h>
// #include <dos.h>
// #include <windows.h>
#include <sstream>
#include <iostream>
#include <math.h>
#include <SFML/Network.hpp>
using namespace  sf;
using namespace  std;
int screenWidth = 800, screenHeigth = 600;
const double  pi = 3.1415926;
const int amountBullet = 200;// ���������� ����
int const amountWalls = 35;// ���������� ����
int const amountBonus = 20;// ���������� �������
int const amountBurst = 20;// ���������� �������
int const timeBurst = 35;// ������������ ������
int const map_Size_X = 20;
int const map_Size_Y = 14;
int const countPacked = 10;
int scorePlayer = 0, scoreBot = 0, starsPlayer = 0, starsBot = 0;
bool pause = false;// ���� ����, ��� ���� ���� �� �������.
bool NoShotFocus = false; // ������ �������� ����� ����� ���� ���� ������ � �����
bool startGame = false;// ���������� �� ����
bool StartConnected = false;// �������� �� �������� ��-���� ���� �� � ��������
bool networkGame = false;
bool mouseCapture = false;// ������ ���� �����������
float MaxDistance = 500;// ������������ ��������� ���������
bool isView3D = true;
bool shakesCamera = false;
bool hitPanzerRed = false, hitPanzerGreen = false;
IpAddress ip = "127.0.0.1";
int port = 2000;
char mode;
enum typePanzer
{
	GREEN, RED
};
sf::RenderWindow window(VideoMode(800, 600), "CONTRA ", Style::Close);
void DrawLine(int x, int y, int x1, int y1, Color color)// ��������� ��������� �����
{
	//������� ������ �����, �� ������� ����� ���������� �����:
	sf::VertexArray lines(sf::Lines, 16 /*���������� �����*/);

	//����� ��� ������ �� ����� ������ ���� �������:
	lines[0].position = sf::Vector2f(x, y);
	lines[1].position = sf::Vector2f(x1, y1);
	//� �.�.

	//����� ��� ������ ����� ��������� ����(��� ����� ��������� ���������):
	lines[0].color = color;
	lines[1].color = color;
	//� �.�.

	//� � ����� ������� ��� �� �����:
	window.draw(lines);
}
double CalcAngle(int x, int y, int ArrivalX, int ArrivalY)// ��������� ������� ���� �� 2� ������
{
	int fdx, fdy;
	double fxy, ff;
	fdx = x - ArrivalX;
	fdy = y - ArrivalY;
	fxy = fdx ? (double)fdy / fdx : 0;
	if (fdx > 0) ff = atan(fxy) * 180.0 / pi - 180;
	else
		ff = atan(fxy) * 180.0 / pi;
	if (x == ArrivalX || fxy == 0)
	{
		if (y > ArrivalY) ff = -90;
		if (y < ArrivalY) ff = 90;
	}
	ff += 90;
	return ff;
}
bool ClickMouseLeft(Event event)// ��������� ��������� ������� ������� ����� ������ ����
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		//if (event.type == Event::MouseButtonPressed)//���� ������ ������� ����
		//			if (event.key.code == Mouse::Left)//� ������ �����
	{
		//std::cout<<"ckick mouse"<<"\n";
		return true;

	}
	return false;
}
double MovingToAngle(double angle, double angle1, bool speedTurn = false)// ������� �������� �������� ���� (����� ��� ���� ��� �� ������ ��� �������) 
{
	int vector = 0;
	//if (vector==0)
	{
		if (fabs(angle1 - angle) >= 180)
		{
			if (angle1 > angle) vector = 2;
			if (angle1 < angle) vector = 1;
		}
		else
		{
			if (angle1 > angle) vector = 1;
			if (angle1 < angle) vector = 2;
		}
	}
	if (angle >= 180) angle -= 360;
	if (angle <= -180) angle += 360;
	double speedRotation = 1;
	if (speedTurn == true)
	{
		if (fabs(angle1 - angle) >= 6) speedRotation = 5;
		if (fabs(angle1 - angle) >= 16) speedRotation = 10;
	}
	if (angle <= angle1 + 1 && angle >= angle1 - 1) { vector = 0; return angle; };
	if (vector == 1) return angle + speedRotation;
	// unit[n].f++;
	if (vector == 2) return angle - speedRotation;
	//unit[n].f--;


}
float ControlMouse()
{
	static int flagBeginProg = false;
	static float dir = 0;
	if (flagBeginProg == false)
	{
		flagBeginProg = true;
		//window.setMouseCursorVisible(false);
	}
	if (Keyboard::isKeyPressed(Keyboard::M))// ������� �� �������
	{
		mouseCapture = !mouseCapture;
		//window.setMouseCursorVisible() // �������� ������
		window.setMouseCursorVisible(!mouseCapture);


		while (Keyboard::isKeyPressed(Keyboard::M))
		{

		}
	}

	if (mouseCapture == true)
	{
		static Vector2i oldMouse = Mouse::getPosition(window);
		Vector2i mousePos = Mouse::getPosition(window);//�������� ���������� �������
		Vector2i winPos = window.getPosition();
		if (pause == false)
		{
			if (oldMouse.x > mousePos.x)
			{
				dir += (oldMouse.x - mousePos.x) / (float)500;
				if (dir > 2 * pi) dir -= 2 * pi;
			}
			else if (oldMouse.x < mousePos.x)
			{
				dir -= (mousePos.x - oldMouse.x) / (float)500;
				if (dir < 0) dir += 2 * pi;
			}
			Mouse::setPosition(Vector2i(winPos.x + screenWidth / 2, winPos.y + screenHeigth / 2));// ���������� ������� � ���������������� ����������  
		}
		//Mouse::setPosition(Vector2i(winPos.x + screenWidth / 2, winPos.y + screenHeigth / 2));// ���������� ������� � ���������������� ����������  
		oldMouse = Mouse::getPosition(window);
		return dir;
	}
	return dir;
}
struct Data
{
	sf::Uint16 key, tip;
	double x, y, angle;
};
Data dataServerRace[countPacked],// ������ ������ �������
dataClientRace[countPacked];// ������ ������ �������
class Server
{

public:
	TcpListener listener;
	TcpSocket client;

	void Listen()
	{
		// bind the listener to a port
		if (listener.listen(2000) != sf::Socket::Done)
		{
			std::cout << "error";
		}

		// accept a new connection
		//sf::TcpSocket client;
		if (listener.accept(client) != sf::Socket::Done)
		{
			std::cout << "error";
		}
		else
		{
			std::cout << "Connect Good" << "\n";
			networkGame = true;
			client.setBlocking(false);
		}
	}
	Data Receive()
	{

		// on receiving side
		sf::Uint16 key, type;
		std::string s;
		double x, y, angle;
		Packet packet;
		Data data;
		data.key = 0;
		if (client.receive(packet) != Socket::NotReady)
		{
			packet >> s >> key >> x >> y >> type >> angle;
			if (s != "")
			{
				std::cout << "client..>" << s << " " << key << " " << x << " " << y << " "
					<< type << " " << angle << std::endl;
				data.key = key;
				data.x = x;
				data.y = y;
				data.tip = type;
				data.angle = angle;
			}
			else
			{
				std::cout << "client not conect" << s << std::endl;
			}
			return data;
		}



	}
	void Send(Uint16 key, double x, double y, Uint16 type, double angle)
	{

		// on sending side
		std::string s = "Hello";
		sf::Packet packet;
		packet << s << key << x << y << type << angle;
		client.send(packet);

	}

};
Server server;
class Client
{

public:
	TcpSocket socket;
	//	TcpListener listener;
	void Connect()
	{
		Socket::Status status = socket.connect(ip, port);
		if (status != Socket::Done)
		{
			std::cout << "error";
		}
		else
		{
			std::cout << "connect good" << "\n";
			networkGame = true;
			socket.setBlocking(false);
		}
	}
	Data Receive()
	{

		// on receiving side
		sf::Uint16 key, type;
		std::string s;
		double x, y, angle;
		Packet packet;
		Data data;
		data.key = 0;
		if (socket.receive(packet) != Socket::NotReady)
		{
			packet >> s >> key >> x >> y >> type >> angle;
			if (s != "")
			{
				std::cout << "server..>" << s << " " << key << " " << x << " " << y << " "
					<< type << " " << angle << std::endl;
				data.key = key;
				data.x = x;
				data.y = y;
				data.tip = type;
				data.angle = angle;
			}
			else
			{
				std::cout << "client not conect" << s << std::endl;
			}
			return data;
		}




	}
	void Send(Uint16 key, double x, double y, Uint16 tip, double angle)
	{

		// on sending side
		std::string s = "Hello";
		sf::Packet packet;
		packet << s << key << x << y << tip << angle;
		socket.send(packet);

	}
};

Client client;
void ServerRace()// ����� ������ � ������� ��������
{
	for (int i = 0; i < countPacked; i++)
	{
		dataServerRace[i] = server.Receive();
	}
}
void ClientRace()// ����� ������ � ������� ��������
{
	for (int i = 0; i < countPacked; i++)
	{
		dataClientRace[i] = client.Receive();
	}
}
class MainMenu
{
	int focus;// ����� ������ ���� �� ������� ������� ��������� ���� 
	bool network;
	int startX, startY;//���������� �� ������� ����� ����������������� ����
	int stepDown;// ��������� ����� �������� ����
	int widthPunct;// ������ ������ ��� ��������� ����
	int numPageMenu;
	int result;
public:
	MainMenu()
	{
		focus = 0;
		network = false;
		startX = 300;
		startY = 165;
		stepDown = 50;
		widthPunct = 210;
		numPageMenu = 0;
		result = 0;
	}
	int Service(Event event)
	{

		Vector2i mousePos = Mouse::getPosition(window);//�������� ���������� �������
		if (network == false)
		{
			focus = 0;
			if (numPageMenu == 0)
			{
				if (mousePos.x > startX && mousePos.x < startX + widthPunct)
				{
					if (mousePos.y > startY && mousePos.y < startY + stepDown) focus = 1;
					if (mousePos.y > startY + stepDown && mousePos.y < startY + stepDown * 2) focus = 2;
					if (mousePos.y > startY + stepDown * 2 && mousePos.y < startY + stepDown * 3) focus = 3;
					if (mousePos.y > startY + stepDown * 3 && mousePos.y < startY + stepDown * 4) focus = 4;
				}
				if (focus == 1 && ClickMouseLeft(event) == true)
				{
					numPageMenu = 2;
					//return 1;
					result = 1;
					while (ClickMouseLeft(event) == true) {}
					return 0;
				}
				if (focus == 2 && ClickMouseLeft(event) == true)
				{
					//network = true;
					numPageMenu = 1;
					result = 2;
					while (ClickMouseLeft(event) == true) {}
					return 0;
					//	return 2;
				}
			}
			if (numPageMenu == 1)
			{
				if (mousePos.x > startX && mousePos.x < startX + widthPunct)
				{
					if (mousePos.y > startY && mousePos.y < startY + stepDown) focus = 1;
					if (mousePos.y > startY + stepDown && mousePos.y < startY + stepDown * 2) focus = 2;
				}
				if (focus == 1 && ClickMouseLeft(event) == true)
				{
					mode = 's';
					while (ClickMouseLeft(event) == true) {}
					numPageMenu = 2;
					return 0;
					//result = 1;
				}
				if (focus == 2 && ClickMouseLeft(event) == true)
				{
					mode = 'c';
					if (result == 2) network = true;
					return result;
					//	while (ClickMouseLeft(event) == true) {}
					//return 0;
				}
			}
			if (numPageMenu == 2)
			{
				if (mousePos.x > startX && mousePos.x < startX + widthPunct)
				{
					if (mousePos.y > startY && mousePos.y < startY + stepDown) focus = 1;
					if (mousePos.y > startY + stepDown && mousePos.y < startY + stepDown * 2) focus = 2;
				}
				if (focus == 1 && ClickMouseLeft(event) == true)
				{
					isView3D = false;
					mouseCapture = false;
					window.setMouseCursorVisible(true);
					if (result == 2) network = true;
					return result;
					//result = 1;
				}
				if (focus == 2 && ClickMouseLeft(event) == true)
				{
					isView3D = true;
					mouseCapture = true;
					window.setMouseCursorVisible(false);
					if (result == 2) network = true;
					return result;
				}
			}
		}
		return 0;
	}
	void Draw(Text text)
	{
		if (numPageMenu == 0)
		{
			if (focus == 1) text.setFillColor(Color::Red); else text.setFillColor(Color::White);
			text.setString("New Game");
			text.setPosition(startX, startY);
			window.draw(text);

			if (focus == 2) text.setFillColor(Color::Red); else text.setFillColor(Color::White);
			text.setString("MultiPlayer");
			text.setPosition(startX, startY + stepDown);
			window.draw(text);

			if (focus == 3) text.setFillColor(Color::Red); else text.setFillColor(Color::White);
			text.setString("About");
			text.setPosition(startX, startY + stepDown * 2);
			window.draw(text);

			if (focus == 4) text.setFillColor(Color::Red); else text.setFillColor(Color::White);
			text.setString("Exit");
			text.setPosition(startX, startY + stepDown * 3);
			window.draw(text);
		}
		if (numPageMenu == 1)
		{
			if (focus == 1) text.setFillColor(Color::Red); else text.setFillColor(Color::White);
			text.setString("Server");
			text.setPosition(startX, startY);
			window.draw(text);

			if (focus == 2) text.setFillColor(Color::Red); else text.setFillColor(Color::White);
			text.setString("Client");
			text.setPosition(startX, startY + stepDown);
			window.draw(text);
		}
		if (numPageMenu == 2)
		{
			if (focus == 1) text.setFillColor(Color::Red); else text.setFillColor(Color::White);
			text.setString("2D");
			text.setPosition(startX, startY);
			window.draw(text);

			if (focus == 2) text.setFillColor(Color::Red); else text.setFillColor(Color::White);
			text.setString("3D");
			text.setPosition(startX, startY + stepDown);
			window.draw(text);
		}
		if (network == true)
		{
			text.setFillColor(Color::Yellow);
			text.setString("Looking in console");
			text.setPosition(startX, startY + stepDown * 5);
			window.draw(text);
		}
	}

};
MainMenu mainmenu;
class Fps
{
	float FPS;
public:

	void Draw(Text text)
	{
		std::ostringstream FPSString;    // �������� ����������
		FPSString << FPS;
		text.setString(FPSString.str());
		text.setPosition(400, 577);
		window.draw(text);

	}
	void Server(int t1, int t2)
	{
		if (t2 - t1 != 0) FPS = 1000000 / (t2 - t1); else FPS = 1000;
		//std::cout<<FPS<<"\n";
	}
};
Fps fps;
class Walls// ����� ����
{

public:
	int size;
	struct Wall// ���� ������
	{
		int x, y;
	};
	Wall wall[amountWalls];
	Walls()// ����������� ����. ������������ �������� �����
	{
		size = 40;
	}
	void Placement()
	{

		for (int i = 0; i < amountWalls; i++)
		{
			int rx = rand() % map_Size_X;
			int ry = rand() % map_Size_Y - 1;
			wall[i].x = rx * 40;
			wall[i].y = ry * 40;
		}

	}
	void DrawWalls()// ���������� �����
	{
		RectangleShape rectangle(sf::Vector2f(size, size));
		rectangle.setFillColor(Color(128, 128, 128));
		for (int i = 0; i < amountWalls; i++)
		{
			rectangle.setPosition(wall[i].x, wall[i].y);
			window.draw(rectangle);
		}
	}
	bool CrossWall(int x, int y, int dx = -1, int dy = -1, bool baruer = false)// ��������� �������� ���������� � �����
	{
		for (int i = 0; i < amountWalls; i++)
		{
			if (dx == -1 && dy == -1)
			{
				//���� �� ������ �������� dx, dy �� ��������� ���� �����
				if (baruer == false)
				{

					if (x >= wall[i].x && x <= wall[i].x + size && y >= wall[i].y && y <= wall[i].y + size) return true;

				}
				else
				{
					// ���� ������ ==������ �� ��������� ��������� �����
					if (x >= wall[i].x - 5 && x <= wall[i].x + size + 5 && y >= wall[i].y - 5 && y <= wall[i].y + size + 5) return true;
				}
			}
			else
			{
				//���� �������� dy, dx  ������� �� ��������� �� ����������� 2 ��������������
				if (x + dx >= wall[i].x && x <= wall[i].x + size && y + dy >= wall[i].y && y <= wall[i].y + size) return true;
			}
		}
		return false;
	}
	void WallsSend(int k)// �������� ��������� ���� � ������� �������
	{


		server.Send(1, wall[k].x, wall[k].y, k, 0);


	}
	int WallsReceive()// ����� ���� � ������� � ���������� ������ �o�������
	{

		Data data;
		data = client.Receive();
		if (data.key == 1)
		{
			wall[data.tip].x = (int)data.x;
			wall[data.tip].y = (int)data.y;
			return data.tip;
		}
		return -1;
	}
};
Walls walls;
class Route //����� ������� ������ ������� ��� �������� ������
{
	int MapSearch[map_Size_X][map_Size_Y];// ������ ������� ������ �������� ���� ������ ����, ������� ������� ����� ������ �� ������ ������

public:
	int length, lengthRoute, arrivalX, arrivalY, beginX, beginY;
	bool being;// ���������� �� �������
	struct PointPuti // ����� ����
	{
		int x, y;
		bool being;
	};
	PointPuti pointRoute[100/*map_Size_X*map_Size_Y*/];
	Route()
	{
		length = 0;
		arrivalX = 0;
		arrivalY = 0;
	}
	void DrawSearchRoute(Text text)
	{
		RectangleShape rectangle(sf::Vector2f(10, 10));
		for (int i = 0; i < map_Size_X; i++)
			for (int j = 0; j < map_Size_Y; j++)
			{
				rectangle.setPosition(i * 40 + 15, j * 40 + 15);
				if (MapSearch[i][j] == -1)
				{
					rectangle.setFillColor(Color(108, 108, 255));
					window.draw(rectangle);
				}
				if (MapSearch[i][j] == -4)
				{
					rectangle.setFillColor(Color(255, 108, 108));

					window.draw(rectangle);
				}
				if (MapSearch[i][j] == -2)
				{
					rectangle.setFillColor(Color(108, 255, 108));

					window.draw(rectangle);
				}


			}
		rectangle.setFillColor(Color(255, 255, 108));
		for (int i = 0; i < length; i++)
			if (pointRoute[i].being == true)
			{

				rectangle.setPosition(pointRoute[i].x * 40 + 15, pointRoute[i].y * 40 + 15);
				window.draw(rectangle);
			}/*
			 for (int i=0;i<map_Size_X;i++) DrawLine(i*40+20,1,i*40+20,600,Color::Red);
			 for (int j=0;j<map_Size_Y;j++) DrawLine(1,j*40+20,800,j*40+20,Color::Red);
			 */


	}
	void CancelRoute()
	{
		lengthRoute = 0;
		length = 0;
		being = 0;
		for (int k = 0; k < 100; k++)
		{
			pointRoute[k].x = 0;
			pointRoute[k].y = 0;
			pointRoute[k].being = false;
		}
		for (int i = 0; i < map_Size_X; i++)
			for (int j = 0; j < map_Size_Y; j++)
			{
				MapSearch[i][j] = 0;
			}
	}
	void PrepareMapForSearchRoute(int x, int y, int arrX, int arrY, int panzerX = -1, int panzerY = -1)// ���������� ������ � ������ �������������� ��� ������ ����
	{

		for (int i = 0; i < map_Size_X; i++)
			for (int j = 0; j < map_Size_Y; j++)
			{
				//������������� ������� ��� ������ ����   
				MapSearch[i][j] = 0;
				for (int k = 0; k < amountWalls; k++)
					if (walls.wall[k].x == i * 40 && walls.wall[k].y == j * 40) // ������� � ������ �����
					{
						MapSearch[i][j] = -1;
						break;
					}
				if (x > i * 40 && x <= i * 40 + 40 && y > j * 40 && y <= j * 40 + 40)
				{
					beginX = i;
					beginY = j;
					MapSearch[i][j] = -2;// ������� � ������ ����� ������ ���� 
				}
				if (arrX > i * 40 && arrX <= i * 40 + 40 && arrY > j * 40 && arrY <= j * 40 + 40)
				{
					arrivalX = i;
					arrivalY = j;
					//  MapPoisk[i][j]=-3;// ������� ����� ����� ����
				}
				if (panzerX != -1 && panzerY != -1)
					if (panzerX + 30 > i * 40 && panzerX<i * 40 + 40 && panzerY + 30>j * 40 && panzerY < j * 40 + 40) MapSearch[i][j] = -4; // ������� ����� ���������� ����������

			}
		for (int i = 0; i < 100; i++)
		{
			pointRoute[i].being = false;
		}
		arrX = arrX;
	}

	void  WaveSpread(int LENGTH)// �������������� ����� ���� �� �,�
	{
		length = LENGTH;

		for (int k = 0; k < length; k++) // ���� ��������������� �����
		{
			for (int i = 0; i < map_Size_X; i++)
				for (int j = 0; j < map_Size_Y; j++)
				{


					if ((MapSearch[i][j] == k && k != 0) || (MapSearch[i][j] == -2 && k == 0))
					{
						/*
						if (MapPoisk[i][j-1]==-3||MapPoisk[i+1][j]==-3
						||MapPoisk[i][j+1]==-3||MapPoisk[i-1][j]==-3)
						{
						dlina=k;
						return 0;
						}
						*/
						if (i - 1 >= 0)
							if (MapSearch[i - 1][j] == 0) MapSearch[i - 1][j] = k + 1;
						if (j + 1 < map_Size_Y)
							if (MapSearch[i][j + 1] == 0) MapSearch[i][j + 1] = k + 1;
						if (i + 1 < map_Size_X)
							if (MapSearch[i + 1][j] == 0) MapSearch[i + 1][j] = k + 1;
						if (j - 1 >= 0)
							if (MapSearch[i][j - 1] == 0) MapSearch[i][j - 1] = k + 1;

						if (arrivalY != 0)
							arrivalY = arrivalY;
					}

				}

		}
	}
	void LoadRoute(int numStep = 0)
	{
		// numstep ���������� ������� ������ ����� ���� �� �������� ���������� ����� ������ ����
		int pointX, pointY;
		lengthRoute = MapSearch[arrivalX][arrivalY];
		pointX = arrivalX;
		pointY = arrivalY;
		pointRoute[lengthRoute].y = pointY;
		pointRoute[lengthRoute].x = pointX;
		pointRoute[lengthRoute].being = true;
		pointRoute[0].y = beginY;
		pointRoute[0].x = beginX;
		pointRoute[0].being = true;
		for (int k = lengthRoute; k > numStep; k--)

		{
			if (MapSearch[pointX][pointY - 1] == k - 1 && pointY > 0)
			{
				pointY--;
				pointRoute[k - 1].y = pointY;
				pointRoute[k - 1].x = pointX;
				pointRoute[k - 1].being = true;
				continue;
			}
			if (MapSearch[pointX + 1][pointY] == k - 1 && pointX < map_Size_X - 1)
			{
				pointX++;
				pointRoute[k - 1].y = pointY;
				pointRoute[k - 1].x = pointX;
				pointRoute[k - 1].being = true;
				continue;
			}
			if (MapSearch[pointX][pointY + 1] == k - 1 && pointY < map_Size_Y - 1)
			{
				pointY++;
				pointRoute[k - 1].y = pointY;
				pointRoute[k - 1].x = pointX;
				pointRoute[k - 1].being = true;
				continue;
			}
			if (MapSearch[pointX - 1][pointY] == k - 1 && pointX > 0)
			{
				pointX--;
				pointRoute[k - 1].y = pointY;
				pointRoute[k - 1].x = pointX;
				pointRoute[k - 1].being = true;
				continue;
			}
		}
		pointX = pointX;
		being = true;// ������� ������ � �������� ��� ������������� ������
		if (Keyboard::isKeyPressed(Keyboard::X))
		{
			pointX = pointX;
		}
	}

};

Route route;
float VectMult(float ax, float ay, float bx, float by)
{
	return (ax * by) - (ay * bx);
}

bool IsCrossing(float a1x, float a1y, float a2x, float a2y, float b1x, float b1y, float b2x, float b2y)
{
	float v1 = VectMult(b2x - b1x, b2y - b1y, a1x - b1x, a1y - b1y);
	float v2 = VectMult(b2x - b1x, b2y - b1y, a2x - b1x, a2y - b1y);
	float v3 = VectMult(a2x - a1x, a2y - a1y, b1x - a1x, b1y - a1y);
	float v4 = VectMult(a2x - a1x, a2y - a1y, b2x - a1x, b2y - a1y);
	return (v1 * v2) <= 0 && (v3 * v4) <= 0;
}
bool LookAcrossWall(int x, int y, int arrivalX, int arrivalY, bool checkDist = false)// ������� ��������� ����� �����
{
	float dx = 0, dy = 0, dist = 0;
	dx = arrivalX - x;
	dy = arrivalY - y;
	dist = sqrt(dx * dx + dy * dy);
	if (checkDist == false || (checkDist == true && dist < MaxDistance))
	{
		for (size_t i = 0; i < amountWalls; i++)
			if (IsCrossing(x, y, arrivalX, arrivalY, walls.wall[i].x, walls.wall[i].y, walls.wall[i].x + walls.size, walls.wall[i].y) ||
				IsCrossing(x, y, arrivalX, arrivalY, walls.wall[i].x + walls.size, walls.wall[i].y, walls.wall[i].x + walls.size, walls.wall[i].y + walls.size) ||
				IsCrossing(x, y, arrivalX, arrivalY, walls.wall[i].x + walls.size, walls.wall[i].y + walls.size, walls.wall[i].x, walls.wall[i].y + walls.size) ||
				IsCrossing(x, y, arrivalX, arrivalY, walls.wall[i].x, walls.wall[i].y + walls.size, walls.wall[i].x, walls.wall[i].y))
			{
				return false;
			}

		return true;
	}
	return false;

	//double angle, pointX = x, pointY = y;
	//angle = CalcAngle(x, y, arrivalX, arrivalY);
	//for (int i = 0; i < 1000; i++)
	//{
	//	pointY += 1 * sin(pi * (angle - 90) / 180);
	//	pointX += 1 * cos(pi * (angle - 90) / 180);
	//	if (walls.CrossWall(pointX, pointY, -1, -1, true)) return false;
	//	if (pointX > arrivalX - 10 && pointX<arrivalX + 10
	//		&& pointY>arrivalY - 10 && pointY < arrivalY + 10) return true;
	//}
}

class Burstes// ����� �������
{
	struct burst// �����
	{
		int x, y, count;
		bool being;
		bool SmallBah;
	};
	burst burst[amountBurst];
public:
	void Draw()// ���������� �����
	{
		for (int i = 0; i < amountBurst; i++)
			if (burst[i].being == true)
			{
				CircleShape shape(burst[i].count);
				if (burst[i].count % 2 == 0) //���� ������ �� �������� ������ ����� ������
				{
					shape.setFillColor(Color(255, 0, 0));

				}
				else
				{
					shape.setFillColor(Color(255, 255, 0));
				}
				shape.setPosition(Vector2f(burst[i].x - burst[i].count, burst[i].y - burst[i].count));
				window.draw(shape);
			}
	}
	void Registration(int xx, int yy, bool smallBabah = false)// ����������� ������
	{
		for (int i = 0; i < amountBurst; i++)
			if (burst[i].being == false)
			{
				burst[i].x = xx;
				burst[i].y = yy;
				burst[i].being = true;
				burst[i].SmallBah = smallBabah;
				break;
			}
	}
	void Service()// ������������ �������
	{
		for (int i = 0; i < amountBurst; i++)
			if (burst[i].being == true)
			{
				if ((burst[i].count < timeBurst && burst[i].SmallBah == false)
					|| burst[i].count < 12 && burst[i].SmallBah == true)
				{
					burst[i].count++;
				}
				else
				{
					burst[i].being = false;
					burst[i].count = 0;
				}
			}
	}
};
Burstes burstes;
class Bonuses// ����� ������
{
public:
	Image bonusesImage; //������� ������ Image (�����������)
	Texture bonusesTexture;//������� ������ Texture (��������)
	Sprite bonusesSprite;
	int size;
	struct Bonus // �����
	{
	public:
		int x, y, type;
		bool being;
		Bonus()
		{
			/*
			x=(rand()%20)*40;
			y=(rand()%15)*40;
			being=true;
			tip= (rand()%3)+1;
			*/

		}
	};
	Bonus bonus[amountBonus];
	Bonuses()
	{
		size = 40;
		bonusesImage.loadFromFile("Bonus.png");//��������� � ���� ����
		bonusesImage.createMaskFromColor(Color(255, 255, 255));

		bonusesTexture.loadFromImage(bonusesImage);//�������� � ���� ������ Image (�����������)
		bonusesSprite.setTexture(bonusesTexture);

	}
	void Draw()// ���������� ������
	{
		for (int i = 0; i < amountBonus; i++)
			if (bonus[i].being == true)
			{
				if (bonus[i].type == 1) bonusesSprite.setTextureRect(IntRect(1, 1, 40, 40));
				if (bonus[i].type == 2) bonusesSprite.setTextureRect(IntRect(40, 1, 40, 40));
				if (bonus[i].type == 3) bonusesSprite.setTextureRect(IntRect(1, 40, 40, 40));
				if (bonus[i].type == 4) bonusesSprite.setTextureRect(IntRect(40, 40, 40, 40));
				bonusesSprite.setPosition(bonus[i].x, bonus[i].y);
				window.draw(bonusesSprite);
			}
	}
	void NewBonus(int x, int y, int tip, bool rezerv = false)// ����������� ������ ������
	{
		int realCountBonus = (rezerv == false ? amountBonus - 15 : amountBonus);
		int i1, i;
		// ��������� ���������� ��� ����� ��� �� ��� ������
		if (rezerv == false)
		{
			i = 0;
			i1 = amountBonus - 15;
		}
		else
		{
			i = amountBonus - 15;
			i1 = amountBonus;
		}
		for (i; i < i1; i++)
			if (bonus[i].being == false)
			{
				bonus[i].x = x / size * size;
				bonus[i].y = y / size * size;
				bonus[i].type = tip;
				bonus[i].being = true;

				break;
			}
		/*
		for (int i=0;i<realKolvoBonus;i++)
		for (int j=0;j<realKolvoBonus;j++)
		if (bonus[i].being==true && bonus[j].being==true && j!=i)
		if(bonus[i].x==bonus[j].x && bonus[i].y==bonus[j].y)
		{
		x=x;
		}
		*/
	}
	int CrossBonus(int x, int y, int dx = -1, int dy = -1)// ��������� �������� ���������� ����� ��� �������������� � ������
	{

		for (int i = 0; i < amountBonus; i++)
			if (bonus[i].being == true)
			{
				if (dx == -1 && dy == -1)
				{
					if (x >= bonus[i].x && x <= bonus[i].x + 40 && y >= bonus[i].y && y <= bonus[i].y + 40) return i;


				}
				else
				{
					if (x + dx >= bonus[i].x && x <= bonus[i].x + 40 && y + dy >= bonus[i].y && y <= bonus[i].y + 40) return i;
				}
			}
		return -1;
	}
	void KillBonus(int n)// ������� �����
	{
		bonus[n].being = false;
	}
};
Bonuses bonuses;

class Bullet// ����� ����
{
public:
	double x, y,// ����������
		angle, // ����
		dx, dy;// �������� ��� ������
	bool being;
	Bullet()
	{
		being = false;
	}
	void Draw()// ���������� ����
	{
		if (being == true)
		{
			CircleShape shape(2);
			// ����� ������ ������ ����
			shape.setFillColor(Color(250, 250, 0));
			shape.setPosition(x, y);
			window.draw(shape);
		}
	}
	void Registration(int xx, int yy, double angle1, double dx1 = 0, double dy1 = 0)// �������� ����
	{
		if (being == false)
		{
			x = xx;
			y = yy;
			angle = angle1;
			dx = dx1;
			dy = dy1;
			being = true;
		}
	}
	void Service()// ������������ ����
	{
		if (being == true)
		{
			y += 15 * sin(pi * (angle - 90) / 180) + dy;
			x += 15 * cos(pi * (angle - 90) / 180) + dx;
			if (y < 0 || y>600 || x < 0 || x>800) being = false;
			if (walls.CrossWall(x, y))
			{
				being = false;
				burstes.Registration(x, y, true);
			}
		}

	}

};
Bullet bullets[amountBullet];
void FlyBullets()// �������� ��������� ������� ����
{
	for (int i = 0; i < amountBullet; i++)
	{
		bullets[i].Service();
	}
}
void Shot(int x, int y, double angle, double dx, double dy)// ����������� ����� ����
{
	for (int i = 0; i < amountBullet; i++)
	{
		if (bullets[i].being == false)
		{
			bullets[i].Registration(x, y, angle, dx, dy);
			break;
		}
	}
}
class Panzer// ����� ���� ������
{
protected:
	float x, y;
	bool being;
	int countKill;// ������� ������� ����� ���� ����
	int pos;
	int size;
	int countAttack,// ������� �����������
		timeAttack;// ����� �����������
	int ammoMagazine;// ������� ��������
	int HP;
	int ARMOR;// �����
	Color color;
	double angle;
	double turnX, turnY, turnX1, turnY1;// ����� ��������� �����

public:
	bool speedTurn;// ������� �������� �����
	Panzer()
	{
		size = 15;
		x = 140;
		y = 100;
		angle = 90;
		countAttack = 0;
		timeAttack = 50;
		ammoMagazine = 15;
		HP = 100;
		ARMOR = 100;
		being = true;
		countKill = 0;
		color = Color::Green;
		speedTurn = false;
	}
	void Draw()// ���������� ���� 
	{
		if (being == true)
		{
			CircleShape shape(size);
			shape.setFillColor(color);
			shape.setPosition(Vector2f(x, y));
			DrawLine(turnX1, turnY1, turnX, turnY, Color::White);
			window.draw(shape);
		}
	}
	void ChangePosition(int newPos)
	{
	}
	void Move(double dx, double dy) // �������� �����
	{
		x += dx;
		y += dy;


	}
	void Turn(double angle)// ������� ����� � ������������ ����
	{
		//int size1=size/3;
		turnY = size * 2 * sin(pi * (angle - 90) / 180) + y + size;
		turnX = size * 2 * cos(pi * (angle - 90) / 180) + x + size;
		turnY1 = size * 3 * sin(pi * (angle - 90) / 180) + y + size;
		turnX1 = size * 3 * cos(pi * (angle - 90) / 180) + x + size;
	}
	float get_TurnX()
	{
		return turnX;
	}
	float get_TurnY()
	{
		return turnY;
	}
	void Control3D(Event event)
	{
		float speed = 1.5;// �������� �������� �����
		float dx = 0, dy = 0;
		static double angleOld = angle;// ���������� ��� ���� ��� �� ���������� ������ �� ���� � ��� ������ ��� ���� ���������
									   // ����� �������� � ������� ������� ������ ���������
									   //if (walls.CrossWall(x, y - speed, size * 2, size * 2) == false)// ���� �� �������� � �����
									   //{
									   //	Move(0, -speed);
									   //	dy. = -speed;

									   //}
		if (mouseCapture == true)
		{
			angle = (180 * -ControlMouse() / pi); ;
		}
		else
		{
			angle = angleOld;
		}

		/*if (angle < -180) angle += 360;
		else if (angle > 180) angle -= 360;*/
		if (Keyboard::isKeyPressed(Keyboard::W)) // �������� �����
		{

			dy = speed * sin(pi * (angle + 90) / 180);
			dx = speed * cos(pi * (angle + 90) / 180);
			Move(dx, dy);

		}
		else if (Keyboard::isKeyPressed(Keyboard::S)) // �������� �����
		{

			dy = -speed * sin(pi * (angle + 90) / 180);
			dx = -speed * cos(pi * (angle + 90) / 180);
			Move(dx, dy);

		}
		else if (Keyboard::isKeyPressed(Keyboard::D)) // �������� �����
		{

			dy = speed * sin(pi * (angle + 90 + 90) / 180);
			dx = speed * cos(pi * (angle + 90 + 90) / 180);
			Move(dx, dy);

		}
		else if (Keyboard::isKeyPressed(Keyboard::A)) // �������� �����
		{

			dy = speed * sin(pi * (angle + 90 - 90) / 180);
			dx = speed * cos(pi * (angle + 90 - 90) / 180);
			Move(dx, dy);

		}

		if (walls.CrossWall(x + dx, y, size*1.5, size*1.5) || x + dx + size / 2>screenWidth || x + dx - size / 2 <0)
		{
			dx = -dx;
			//dy = -dy;
			Move(dx, 0);
		}
		if (walls.CrossWall(x, y + dy, size*1.5, size*1.5) || y + dy + size / 2 > screenHeigth - 40 - size / 2 || y + dy - size / 2 < 0)
		{
			//dx = -dx;
			dy = -dy;
			Move(0, dy);
		}

		//Vector2i mousePos = Mouse::getPosition(window);//�������� ���������� �������

		//angleMouse = CalcAngle(x + size, y + size, mousePos.x, mousePos.y);// ������ ���� ����
		//cout<<angle<<"\n";
		if (NoShotFocus == false)
			if (ClickMouseLeft(event) && countAttack >= timeAttack && ammoMagazine > 0)// ������� ��������
			{
				double angleVustrel = angle + 180/*+(rand() % 7) - 4*/;
				//vustrel(turnX,turnY,angle+(rand() % 7)-4,dx,dy);
				Shot(turnX, turnY, angleVustrel, 0, 0);
				ammoMagazine--;
				countAttack = 0;
				if (networkGame == true)
				{
					server.Send(5, turnX, turnY, 0, angleVustrel);
				}
			}
		// ������� ����� ���� ���� ��������� ����
		//angle = MovingToAngle(angle, angleMouse, speedTurn);
		Turn(angle + 180);
		cout << angle << "   " << angle + 90 << '\n';
		if (networkGame == true)
		{

			if (mode == 's')
			{
				if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::D)
					|| Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::W))
				{
					server.Send(3, x, y, 0, 0);
				}
				if (angle >= angleOld + 1 || angle <= angleOld - 1)
				{
					server.Send(4, 0, 0, 0, angle + 180);
					angleOld = angle;
				}
			}
		}
		else
		{
			angleOld = angle;
		}
	}
	virtual void Control(Event event)//������� ���������� ������ � ����������
	{
		double angleMouse, // ���� �������� ���� ������� ����
			speed = 1.5,// �������� �������� �����
			dx = 0, dy = 0;// �������� �������� �� ��������� � �����������
		static double angleOld = angle;// ���������� ��� ���� ��� �� ���������� ������ �� ���� � ��� ������ ��� ���� ���������
		int vector = 0;
		if (Keyboard::isKeyPressed(Keyboard::W)) // �������� �����
		{
			if (walls.CrossWall(x, y - speed, size * 2, size * 2) == false)// ���� �� �������� � �����
			{
				Move(0, -speed);
				dy = -speed;

			}
		}
		if (Keyboard::isKeyPressed(Keyboard::D)) // �������� ������
		{
			if (walls.CrossWall(x + speed, y, size * 2, size * 2) == false)
			{
				Move(speed, 0);
				dx = speed;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::S)) // �������� ����
		{
			if (walls.CrossWall(x, y + speed, size * 2, size * 2) == false)
			{
				Move(0, speed);
				dy = speed;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::A)) // ������� �����
		{
			if (walls.CrossWall(x - speed, y, size * 2, size * 2) == false)
			{
				Move(-speed, 0);
				dx -= speed;
			}
		}
		if (x <= 0 || x + size * 2 >= 800 || y <= 0 || y + size * 2 >= 560)// ���� �������� � ������� �������� ����
		{
			Move(-dx, -dy);
		}
		Vector2i mousePos = Mouse::getPosition(window);//�������� ���������� �������

		angleMouse = CalcAngle(x + size, y + size, mousePos.x, mousePos.y);// ������ ���� ����
																		   //cout<<angle<<"\n";
		if (NoShotFocus == false)
			if (ClickMouseLeft(event) && countAttack >= timeAttack && ammoMagazine > 0)// ������� ��������
			{
				double angleVustrel = angle + (rand() % 7) - 4;
				//vustrel(turnX,turnY,angle+(rand() % 7)-4,dx,dy);
				Shot(turnX, turnY, angleVustrel, 0, 0);
				ammoMagazine--;
				countAttack = 0;
				if (networkGame == true)
				{
					server.Send(5, turnX, turnY, 0, angleVustrel);
				}
			}
		// ������� ����� ���� ���� ��������� ����
		angle = MovingToAngle(angle, angleMouse, speedTurn);
		Turn(angle);
		cout << angle << '\n';
		if (networkGame == true)
		{

			if (mode == 's')
			{
				if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::D)
					|| Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::W))
				{
					server.Send(3, x, y, 0, 0);
				}
				if (angle >= angleOld + 1 || angle <= angleOld - 1)
				{
					server.Send(4, 0, 0, 0, angle);
					angleOld = angle;
				}
			}
		}
	}
	bool CrossMe(int xx, int yy, int dx1 = -1, int dy1 = -1)//���������� ����� ��� �������������� � �����
	{

		if (dx1 == -1 && dy1 == -1)
		{
			if (xx >= x && xx <= x + size * 2 && yy >= y && yy <= y + size * 2) return true;
		}
		else
		{
			if (xx + dx1 >= x && xx <= x + size && yy + dy1 >= y && yy <= y + size) return true;
		}

		return false;
	}

	void Servis()// ��������� ��� ������������ �����
	{
		if (being == true)
		{
			if (countAttack < timeAttack)	countAttack++;
		}
		else
		{
			if (countKill < timeBurst)// ���� ���� ����������� 
			{
				countKill++;
			}
			else
			{
				countKill = 0;
				being = true;
			}

		}

	}
	void PanzerRace() // ��������� ��������� ������� ������ � ������� ������ ���������� � �������
	{

		Data data;
		for (int i = 0; i < countPacked; i++)
		{
			data = dataClientRace[i];
			if (data.key == 3)
			{
				x = data.x;
				y = data.y;
				Turn(angle);
			}
			if (data.key == 4)
			{
				angle = data.angle;
				Turn(angle);
			}
			if (data.key == 5)
			{
				Shot(data.x, data.y, data.angle, 0, 0);
			}
		}

	}
	void KIlled()
	{
		being = false;
	}
	int Get_X()
	{
		return x;
	}
	int Get_Y()
	{
		return y;
	}
	void Put_X(int xx)
	{
		x = xx;
	}
	void Put_Y(int yy)
	{
		y = yy;
	}
	int Get_Size()
	{
		return size;
	}
	int Get_Pylu()
	{
		return ammoMagazine;
	}

	void Add_Pylu(int value)
	{
		ammoMagazine += value;
		if (ammoMagazine > 30) ammoMagazine = 30;
	}
	void Put_Pylu(int value)
	{
		ammoMagazine = value;
	}
	int Get_HP()
	{
		return HP;
	}

	void Add_HP(int value)
	{
		HP += value;
		if (HP > 100) HP = 100;
		if (HP < 0) HP = 0;
	}
	void Put_HP(int value)
	{
		HP = value;
	}

	int Get_Brony()
	{
		return ARMOR;
	}

	void Add_Brony(int value)
	{
		ARMOR += value;
		if (ARMOR > 100) ARMOR = 100;
		if (ARMOR < 0) ARMOR = 0;
	}
	void Put_Brony(int value)
	{
		ARMOR = value;
	}
	int Get_ChetAttack()
	{
		return countAttack;
	}
	int Get_TimeAttack()
	{
		return timeAttack;
	}
	void Add_TimeAttack(int value)
	{
		timeAttack += value;
		if (timeAttack > 100) timeAttack = 100;
		if (timeAttack < 3) timeAttack = 3;
		countAttack = timeAttack;
	}
	int Get_Being()
	{
		return being;
	}
};
Panzer panzer;
class PanzerBot : public Panzer// ����� ����� ���� ������������ �� ����� ������
{
	int vector;// ������ �������� �����
	int numStep;// ����� ���������� ������, ����� �� ����
	double speed, dx, dy;// �������� �������� � �������� �� � � �
	bool looking;// ����� �� ��� ������ 

	bool motion;// ������ �� ��� �� ������ � ������ ����
	bool normalizePuty;// ����������� �� ���� ���� 
public:
	bool motionToPuty;// �������� �� ��� �� ����
	PanzerBot()
	{
		x = (int)10 * 40 + 5;
		y = (int)10 * 40 + 5;
		color = Color::Red;
		vector = (rand() % 4) + 1;
		looking = false;
		countAttack = 0;
		timeAttack = 50;
		HP = 100;
		ARMOR = 100;
		speed = 1.5;
		motionToPuty = false;

	}
	void CancelMovingToRoute()// �������� ������� ���� �� ����
	{
		motionToPuty = false;
		numStep = 0;
		route.being = false;
		route.CancelRoute();
	}
	void RegPointAim(int arrivalX, int arrivalY)// ����������� ����� ���� ������ �������� ���
	{
		CancelMovingToRoute();
		numStep = 0;
		route.PrepareMapForSearchRoute(x + size, y + size, arrivalX, arrivalY,
			panzer.Get_X() + panzer.Get_Size(), panzer.Get_Y() + panzer.Get_Size());
		route.WaveSpread(45);
		route.LoadRoute();
		motionToPuty = true;
		BeginNormalize();
		vector = 0;
	}
	void RefreshRoute(int arrivalX, int arrivalY)
	{
		Vector2i mousePos = Mouse::getPosition(window);//�������� ���������� �������
		route.PrepareMapForSearchRoute(x + size, y + size, arrivalX, arrivalY,
			panzer.Get_X() + panzer.Get_Size(), panzer.Get_Y() + panzer.Get_Size());
		/*marshrut.LoadMapPoisk(x+size,y+size,arrivalX,arrivalY,
		mousePos.x+panzer.Get_Size(),mousePos.y+panzer.Get_Size());
		*/
		route.WaveSpread(45);
		route.LoadRoute();
		//	if (numStep!=0)
		numStep = 1;
	}
	void BonusControl(bool refresh = false)
	{
		static int selectBonusX = 0, selectBonusY = 0, selectBonusNum = 0;
		static bool selectBonusBeing = false;
		for (int i = 0; i < amountBonus; i++)
			if (bonuses.bonus[i].being == true)
			{
				if ((bonuses.bonus[i].type == 2 || bonuses.bonus[i].type == 3)
					&& selectBonusBeing == false)
				{
					selectBonusX = bonuses.bonus[i].x + 20;
					selectBonusY = bonuses.bonus[i].y + 20;
					selectBonusBeing = true;
					selectBonusNum = i;
					RegPointAim(selectBonusX, selectBonusY);
				}
			}
		if (bonuses.bonus[selectBonusNum].being == false)
		{
			selectBonusBeing = false;
			CancelMovingToRoute();
		}
		if (selectBonusBeing == true && motionToPuty == true)
		{
			int flag = 0;
			for (int i = 0; i < 100; i++)
				if (route.pointRoute[i].being == true)
				{
					if ((x + size >= route.pointRoute[i].x * 40 + 20 - speed
						&& x + size < route.pointRoute[i].x * 40 + 20 + speed)

						&& (y + size >= route.pointRoute[i].y * 40 + 20 - speed
							&& y + size < route.pointRoute[i].y * 40 + 20 + speed))
					{
						flag = 1;
						break;
					}

				}
			if (flag == 1 || refresh == true)
			{
				RefreshRoute(selectBonusX, selectBonusY);
				//		normalizePuty=true;
			}
		}
	}
	void ToGoBonus()// ��������� ������� ����, ����� ������ �����
	{

		for (int i = 0; i < amountBonus; i++)
			if (bonuses.bonus[i].being == true)
			{
				if (motionToPuty == false)
				{
					if (LookAcrossWall(x + size, y + size, bonuses.bonus[i].x + 20, bonuses.bonus[i].y + 20))
					{
						if (bonuses.bonus[i].x + 20 >= x + size - 2 && bonuses.bonus[i].x + 20 <= x + size + 2)// ���� ����� ���� ��� ����
						{
							if (bonuses.bonus[i].y + 20 >= y + size) vector = 3;
							if (bonuses.bonus[i].y + 20 <= y + size) vector = 1;
						}
						if (bonuses.bonus[i].y + 20 >= y + size - 2 && bonuses.bonus[i].y + 20 <= y + size + 2)//���� ����� ����� ��� ������
						{
							if (bonuses.bonus[i].x + 20 >= x + size) vector = 2;
							if (bonuses.bonus[i].x + 20 <= x + size) vector = 4;
						}
					}

				}

			}
		BonusControl();
	}
	void MovingToKeyboard()// ��������� ����� � ����������
	{
		if (Keyboard::isKeyPressed(Keyboard::Up)) // �������� �����
		{

			Move(0, -speed);

		}
		if (Keyboard::isKeyPressed(Keyboard::Right)) // �������� ������
		{
			Move(speed, 0);

		}
		if (Keyboard::isKeyPressed(Keyboard::Down)) // �������� ����
		{

			Move(0, speed);

		}
		if (Keyboard::isKeyPressed(Keyboard::Left)) // ������� �����
		{

			Move(-speed, 0);

		}
		if (Keyboard::isKeyPressed(Keyboard::Space)) // ������� �����
		{
			vector = 0;
		}
	}
	void MoveToVector()// ����������� ���� �� �������� �������
	{
		if (vector == 1) // �������� �����
		{
			if (walls.CrossWall(x, y - speed, size * 2, size * 2) == false)
			{
				Move(0, -speed);
				dy = -speed;

			}
			else vector = (rand() % 4) + 1;
		}
		if (vector == 2) // �������� ������
		{
			if (walls.CrossWall(x + speed, y, size * 2, size * 2) == false)
			{
				Move(speed, 0);
				dx = speed;
			}
			else vector = (rand() % 4) + 1;
		}
		if (vector == 3) // �������� ����
		{
			if (walls.CrossWall(x, y + speed, size * 2, size * 2) == false)
			{
				Move(0, speed);
				dy = speed;
			}
			else vector = (rand() % 4) + 1;
		}
		if (vector == 4)// �������� ������
		{
			if (walls.CrossWall(x - speed, y, size * 2, size * 2) == false)
			{
				Move(-speed, 0);
				dx -= speed;
			}
			else vector = (rand() % 4) + 1;
		}
		if (vector != 0)
		{
			if (networkGame == true)
			{
				if (mode == 'c')
				{
					client.Send(3, x, y, 0, 0);
				}

			}
		}
	}



	bool NormalizeX()// ������ ���� �������������� �� �
	{
		if (x + size >= route.pointRoute[numStep].x * 40 + 20 - speed
			&& x + size <= route.pointRoute[numStep].x * 40 + 20 + speed) return true;
		return false;
	}
	bool NormalizeY()// ������� ���� �������������� �� �
	{
		if (y + size >= route.pointRoute[numStep].y * 40 + 20 - speed
			&& y + size <= route.pointRoute[numStep].y * 40 + 20 + speed) return true;
		return false;
	}
	void BeginNormalize()// ������ ������������� �������� � ����������� ����� ������
	{
		if (NormalizeX() != true || NormalizeY() != true)
		{
			normalizePuty = true;
		}
	}
	// ������� ������� ���������� �� ����� ����� ������ ����� ��� ���� ����� ��� �� �������� � �����
	void NormalizeMovingToRoute()
	{
		/*
		if (NormalizeX()==true||NormalizeY()==true)
		{
		normalizePuty=false;
		}*/
		if (numStep <= route.lengthRoute)
		{
			if (route.pointRoute[numStep].being == true)
				if (motionToPuty == true && route.being == true
					&& normalizePuty == true
					)
				{
					if ((route.pointRoute[numStep].y * 40 + 20 <= y + size
						|| route.pointRoute[numStep].y * 40 + 20 >= y + size)
						&& !(x + size >= route.pointRoute[numStep].x * 40 + 20 - speed
							&& x + size < route.pointRoute[numStep].x * 40 + 20 + speed))// ���� ����� ���� ��������� ���� ��� ����
					{
						if (route.pointRoute[numStep].x * 40 + 20 <= x + size)		vector = 4;
						if (route.pointRoute[numStep].x * 40 + 20 > x + size)		vector = 2;
						if (NormalizeX() == true)
						{
							normalizePuty = false;
							vector = 0;

						}
						//	return;
					}

					if ((route.pointRoute[numStep].x * 40 + 20 <= x + size
						|| route.pointRoute[numStep].x * 40 + 20 >= x + size)
						&& !(y + size >= route.pointRoute[numStep].y * 40 + 20 - speed
							&& y + size < route.pointRoute[numStep].y * 40 + 20 + speed)) // ���� ����� ���� ��������� ����� ��� ������
					{
						if (route.pointRoute[numStep].y * 40 + 20 <= y + size)		vector = 1;
						if (route.pointRoute[numStep].y * 40 + 20 > y + size)		vector = 3;
						if (NormalizeY() == true)
						{

						}
						//	return;
					}
					if ((x + size >= route.pointRoute[numStep].x * 40 + 20 - speed
						&& x + size < route.pointRoute[numStep].x * 40 + 20 + speed)

						&& (y + size >= route.pointRoute[numStep].y * 40 + 20 - speed
							&& y + size < route.pointRoute[numStep].y * 40 + 20 + speed))
					{
						normalizePuty = false;
						vector = 0;

					}

				}
		}

	}

	void MovingToRoute()// ������� �������� ���� �� ����
	{

		if (route.pointRoute[numStep].being == true)
			if (motionToPuty == true && route.being == true)
			{
				//	vector=1;
				if (x + size >= route.pointRoute[numStep].x * 40 + 20 - speed
					&& x + size < route.pointRoute[numStep].x * 40 + 20 + speed
					&& route.pointRoute[numStep].y * 40 + 20 < y + size)
				{
					vector = 1;
				}
				if (y + size >= route.pointRoute[numStep].y * 40 + 20 - speed
					&& y + size<route.pointRoute[numStep].y * 40 + 20 + speed
					&& route.pointRoute[numStep].x * 40 + 20>x + size)
				{
					vector = 2;
				}
				if (x + size >= route.pointRoute[numStep].x * 40 + 20 - speed
					&& x + size<route.pointRoute[numStep].x * 40 + 20 + speed
					&& route.pointRoute[numStep].y * 40 + 20>y + size)
				{
					vector = 3;
				}
				if (y + size >= route.pointRoute[numStep].y * 40 + 20 - speed
					&& y + size < route.pointRoute[numStep].y * 40 + 20 + speed
					&& route.pointRoute[numStep].x * 40 + 20 < x + size)
				{
					vector = 4;
				}
			}
		if (x + size > route.pointRoute[numStep].x * 40 + 20 - speed
			&& x + size<route.pointRoute[numStep].x * 40 + 20 + speed
			&& y + size>route.pointRoute[numStep].y * 40 + 20 - speed
			&& y + size < route.pointRoute[numStep].y * 40 + 20 + speed)
		{
			numStep++;
		}
		int dlinaPuti = route.lengthRoute;
		if (x + size > route.pointRoute[dlinaPuti].x * 40 + 20 - speed
			&& x + size<route.pointRoute[dlinaPuti].x * 40 + 20 + speed
			&& y + size>route.pointRoute[dlinaPuti].y * 40 + 20 - speed
			&& y + size < route.pointRoute[dlinaPuti].y * 40 + 20 + speed)

		{
			CancelMovingToRoute();
		}

		if (Keyboard::isKeyPressed(Keyboard::Z))
		{
			//	bonuses.NewBonus(rand()%800,rand()%541,1);
			sleep(milliseconds(10));
		}
	}
	void ControlAttack()// ������� ������� ��������� ������� ����
	{
		double angleAim = 0;
		static double angleOld = angle;// ����� ��� ���� ����� ���������� ��������� �� ���� �������� �����
		if (panzer.Get_Being() == true)// ������� ����� ����
			if (ammoMagazine > 0)// ���� ���� ������ 0
				if (LookAcrossWall(x + size, y + size, panzer.Get_X() + panzer.Get_Size(),
					panzer.Get_Y() + panzer.Get_Size(), isView3D == true ? true : false))// ���� ����� ���� ������
				{
					// ������ ���� ��� �����
					angleAim = CalcAngle(x + size, y + size, panzer.Get_X() + panzer.Get_Size(), panzer.Get_Y() + panzer.Get_Size());
					vector = 0;// ��������� �������� �����
					looking = true;
					angle = MovingToAngle(angle, angleAim);// �������� ����� ����� ����� ����
														   //angle=angleAim;
					if (angle >= angleAim - 1 && angle <= angleAim + 1 && countAttack >= timeAttack && ammoMagazine > 0)// ������� �������� ����
					{
						double angleVustrel = angle + (rand() % 7) - 4;
						Shot(turnX, turnY, angleVustrel, 0, 0);
						ammoMagazine--;
						countAttack = 0;
						if (networkGame == true)
						{
							client.Send(5, turnX, turnY, 0, angleVustrel);
						}
					}
					if (networkGame == true)
					{
						if (angle >= angleOld + 1 || angle <= angleOld - 1)
						{
							//client.Send(4,0,0,0,angle);
							angleOld = angle;
						}
					}

				}
				else
				{
					if (looking == true)// ���� ����� ���� � ���� ���������
					{
						vector = (rand() % 4) + 1;
						looking = false;
					}

				}
		if (vector == 0 && looking == true && ammoMagazine <= 0)// ���� ��� ����� ������ �� � ���� ����������� �������
		{
			vector = (rand() % 4) + 1;
			looking = false;
		}
		Turn(angle);// �������� ����� ���� � ����������� ����� ����
	}
	void AutoControl()// ������� ���������� ����
	{
		int vector1 = vector;
		dx = 0;
		dy = 0;
		if (motionToPuty == false) 	ControlAttack();

		if (rand() % 200 == 1) vector = (rand() % 4) + 1;// ������� ������ �������o � ������������ 1 �� 200
		MovingToRoute();
		ToGoBonus();

		NormalizeMovingToRoute();
		MoveToVector();
		if (x <= 0 || x + size * 2 >= 800 || y <= 0 || y + size * 2 >= 560)// ���� �������� � ������� ���� ����
		{
			do// ������ ������ ���� �� ����� ����������� �������
			{
				vector = (rand() % 4) + 1;
			} while (vector1 == vector);
			vector1 = vector;
			Move(-dx, -dy);
		}
		if (motionToPuty == true) Turn(angle);

	}
	void Control(Event event)//������� ���������� ������ � ����������
	{
		double angleMouse, // ���� �������� ���� ������� ����
			speed = 1.5,// �������� �������� �����
			dx = 0, dy = 0;// �������� �������� �� ��������� � �����������
		static double angleOld = angle;// ���������� ��� ���� ��� �� ���������� ������ �� ���� � ��� ������ ��� ���� ���������
		int vector = 0;
		if (Keyboard::isKeyPressed(Keyboard::W)) // �������� �����
		{
			if (walls.CrossWall(x, y - speed, size * 2, size * 2) == false)// ���� �� �������� � �����
			{
				Move(0, -speed);
				dy = -speed;

			}
		}
		if (Keyboard::isKeyPressed(Keyboard::D)) // �������� ������
		{
			if (walls.CrossWall(x + speed, y, size * 2, size * 2) == false)
			{
				Move(speed, 0);
				dx = speed;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::S)) // �������� ����
		{
			if (walls.CrossWall(x, y + speed, size * 2, size * 2) == false)
			{
				Move(0, speed);
				dy = speed;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::A)) // ������� �����
		{
			if (walls.CrossWall(x - speed, y, size * 2, size * 2) == false)
			{
				Move(-speed, 0);
				dx -= speed;
			}
		}
		if (x <= 0 || x + size * 2 >= 800 || y <= 0 || y + size * 2 >= 560)// ���� �������� � ������� �������� ����
		{
			Move(-dx, -dy);
		}
		Vector2i mousePos = Mouse::getPosition(window);//�������� ���������� �������

		angleMouse = CalcAngle(x + size, y + size, mousePos.x, mousePos.y);// ������ ���� ����
																		   //cout<<angle<<"\n";
		if (NoShotFocus == false)
			if (ClickMouseLeft(event) && countAttack >= timeAttack && ammoMagazine > 0)// ������� ��������
			{
				double angleVustrel = angle + (rand() % 7) - 4;
				//vustrel(turnX,turnY,angle+(rand() % 7)-4,dx,dy);
				Shot(turnX, turnY, angleVustrel, 0, 0);
				ammoMagazine--;
				countAttack = 0;
				if (networkGame == true)
				{
					client.Send(5, turnX, turnY, 0, angleVustrel);
				}
			}
		// ������� ����� ���� ���� ��������� ����
		angle = MovingToAngle(angle, angleMouse, speedTurn);
		Turn(angle);

		if (networkGame == true)
		{

			if (mode == 'c')
			{
				if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::D)
					|| Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::W))
				{
					client.Send(3, x, y, 0, 0);
				}
				if (angle >= angleOld + 1 || angle <= angleOld - 1)
				{
					client.Send(4, 0, 0, 0, angle);
					angleOld = angle;
				}
			}
		}
	}
	void Control3D(Event event)
	{
		float speed = 1.5;// �������� �������� �����
		float dx = 0, dy = 0;
		static double angleOld = angle;// ���������� ��� ���� ��� �� ���������� ������ �� ���� � ��� ������ ��� ���� ���������
									   // ����� �������� � ������� ������� ������ ���������
									   //if (walls.CrossWall(x, y - speed, size * 2, size * 2) == false)// ���� �� �������� � �����
									   //{
									   //	Move(0, -speed);
									   //	dy. = -speed;

									   //}
		angle = (180 * -ControlMouse() / pi); ;
		/*if (angle < -180) angle += 360;
		else if (angle > 180) angle -= 360;*/
		if (mouseCapture == true)
		{
			angle = (180 * -ControlMouse() / pi); ;
		}
		else
		{
			angle = angleOld;
		}

		if (Keyboard::isKeyPressed(Keyboard::W)) // �������� �����
		{

			dy = speed * sin(pi * (angle + 90) / 180);
			dx = speed * cos(pi * (angle + 90) / 180);
			Move(dx, dy);

		}
		else if (Keyboard::isKeyPressed(Keyboard::S)) // �������� �����
		{

			dy = -speed * sin(pi * (angle + 90) / 180);
			dx = -speed * cos(pi * (angle + 90) / 180);
			Move(dx, dy);

		}
		else if (Keyboard::isKeyPressed(Keyboard::D)) // �������� �����
		{

			dy = speed * sin(pi * (angle + 90 + 90) / 180);
			dx = speed * cos(pi * (angle + 90 + 90) / 180);
			Move(dx, dy);

		}
		else if (Keyboard::isKeyPressed(Keyboard::A)) // �������� �����
		{

			dy = speed * sin(pi * (angle + 90 - 90) / 180);
			dx = speed * cos(pi * (angle + 90 - 90) / 180);
			Move(dx, dy);

		}

		if (walls.CrossWall(x + dx, y, size * 2, size * 2))
		{
			dx = -dx;
			//dy = -dy;
			Move(dx, 0);
		}
		if (walls.CrossWall(x, y + dy, size * 2, size * 2))
		{
			//dx = -dx;
			dy = -dy;
			Move(0, dy);
		}

		if (walls.CrossWall(x + dx, y, size * 1.5, size * 1.5) || x + dx + size / 2 > screenWidth || x + dx - size / 2 < 0)
		{
			dx = -dx;
			//dy = -dy;
			Move(dx, 0);
		}
		if (walls.CrossWall(x, y + dy, size * 1.5, size * 1.5) || y + dy + size / 2 > screenHeigth - 40 - size / 2 || y + dy - size / 2 < 0)
		{
			//dx = -dx;
			dy = -dy;
			Move(0, dy);
		}

		//Vector2i mousePos = Mouse::getPosition(window);//�������� ���������� �������

		//angleMouse = CalcAngle(x + size, y + size, mousePos.x, mousePos.y);// ������ ���� ����
		//cout<<angle<<"\n";
		if (NoShotFocus == false)
			if (ClickMouseLeft(event) && countAttack >= timeAttack && ammoMagazine > 0)// ������� ��������
			{
				double angleVustrel = angle + 180/*+(rand() % 7) - 4*/;
				//vustrel(turnX,turnY,angle+(rand() % 7)-4,dx,dy);
				Shot(turnX, turnY, angleVustrel, 0, 0);
				ammoMagazine--;
				countAttack = 0;
				if (networkGame == true && mode == 'c')
				{
					client.Send(5, turnX, turnY, 0, angleVustrel);
				}
			}
		// ������� ����� ���� ���� ��������� ����
		//angle = MovingToAngle(angle, angleMouse, speedTurn);
		Turn(angle + 180);
		//	cout << angle << "   " << angle + 90 << '\n';
		if (networkGame == true)
		{

			if (mode == 'c')
			{
				if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::D)
					|| Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::W))
				{
					client.Send(3, x, y, 0, 0);
				}
				if (angle >= angleOld + 1 || angle <= angleOld - 1)
				{
					client.Send(4, 0, 0, 0, angle + 180);
					angleOld = angle;
				}
			}
		}
	}
	void PanzerBotRace() // ��������� ��������� ����� � ������� ��������� ������ � �������
	{

		Data data;
		for (int i = 0; i < countPacked; i++)
		{
			data = dataServerRace[i];
			if (data.key == 3)
			{
				x = data.x;
				y = data.y;
				Turn(angle);
			}
			if (data.key == 4)
			{
				angle = data.angle;
				Turn(angle);
			}
			if (data.key == 5)
			{
				Shot(data.x, data.y, data.angle, 0, 0);
			}
		}

	}
};
PanzerBot panzerBot;
Vector2i NewCoordinateBonus()// ������ ����� ��������� ��� ������
{
	int x, y;
	Vector2i vect;
	do
	{
		x = (rand() % 740) + 30;
		y = (rand() % 530) + 30;
	} while (walls.CrossWall(x, y) == true || bonuses.CrossBonus(x, y) != -1
		|| panzer.CrossMe(x - 40, y - 40, 80, 80) == true || panzerBot.CrossMe(x - 40, y - 40, 80, 80) == true);
	vect.x = x;
	vect.y = y;
	return vect;
}
void NewBonusGame()// ���������� ������� ���� � �������� 
{
	static int chet = 0, chet1 = 0;
	int x, y, kolvoBonusPylu = 0, kolvoBonusPricel = 0;
	Vector2i pos;
	if (chet > 270)
	{
		if (chet > 270) chet = 0;
		pos = NewCoordinateBonus();

		//bonuses.NewBonus(x,y,(rand()%2)+2);
		if (networkGame == false)	bonuses.NewBonus(pos.x, pos.y, 1);
		if (networkGame == true)
		{
			if (mode == 's')
			{
				bonuses.NewBonus(pos.x, pos.y, 1);
				server.Send(2, pos.x, pos.y, 1, 0);
			}
		}
	}
	// ���� 5 ������� ���� ���������� �� �� ������� ����� ��������� ������ ������
	for (int i = 0; i < amountBonus; i++)
	{

		if (bonuses.bonus[i].being == true)
			if (bonuses.bonus[i].type == 1) kolvoBonusPylu++;
	}
	if (kolvoBonusPylu != 5) chet++; else chet = 0;
	//if (chet1>1000 )
	//{
	//	if (chet>270) chet=0;
	//	pos=NewCoordinateBonus();

	//	//bonuses.NewBonus(x,y,(rand()%2)+2);
	//	//bonuses.NewBonus(pos.x,pos.y,4,true);
	//	if (networkGame==false)	bonuses.NewBonus(pos.x,pos.y,4,true);
	//	if (networkGame==true)
	//	{
	//		if (mode=='s')
	//		{
	//			bonuses.NewBonus(pos.x,pos.y,4,true);
	//			server.Send(2,pos.x,pos.y,4,0);
	//		}
	//	}
	//}
	//for (int i=0 ;i<kolvoBonus;i++)
	//{

	//	if (bonuses.bonus[i].being==true)
	//	if (bonuses.bonus[i].tip==4) kolvoBonusPricel++;
	//}
	//if (kolvoBonusPricel==0 && panzer.speedTurn==false) chet1++ ;else chet1=0;
}
void ClientBonusRace()// ����� ������� � ������� � ���������� �� �� ����� �������
{
	Data data;
	for (int i = 0; i < countPacked; i++)
	{
		data = dataClientRace[i];
		if (data.key == 2)
		{
			if (data.tip == 1) bonuses.NewBonus((int)data.x, (int)data.y, 1);
			else
			{
				bonuses.NewBonus((int)data.x, (int)data.y, data.tip, true);
			}
			dataClientRace[i].key = 0;
		}
	}
}
class GameInterface// ����� ������ ������� ���������� ����� �����
{
	int HP, BRONY, BULLET, TimeAttack, CountAttack;
	bool bonusAim;//����� ������
	Image interfaceImage;
	Texture interfaceTexture;
	Sprite interfaceSprite;
	int mixingUp;
public:
	GameInterface()
	{

		interfaceImage.loadFromFile("InterfaceImage.png");//��������� � ���� ����
		interfaceImage.createMaskFromColor(Color(255, 255, 255));

		interfaceTexture.loadFromImage(interfaceImage);//�������� � ���� ������ Image (�����������)
		interfaceSprite.setTexture(interfaceTexture);
		bonusAim = false;
		mixingUp = 40;
	}
	void LoadData(int hp, int brony, int pylu1, int chetattack, int timeattack)// ������� ������
	{
		HP = hp;
		BRONY = brony;
		BULLET = pylu1;
		TimeAttack = timeattack;
		CountAttack = chetattack;

	}
	void LoadDataBonus(bool bonusPric)
	{
		bonusAim = bonusPric;
	}
	void DrawAim()
	{
		Color color = Color::White;
		static int count = 0;
		int maxCount = 10;
		if ((hitPanzerRed == true && networkGame == false) || ((hitPanzerGreen == true || hitPanzerRed == true) && networkGame == true))
		{
			count = count;
			if (count < maxCount)
			{
				color = count % 2 == 0 ? Color::Blue : Color(0, 255, 255);
				count++;
			}
			else
			{
				count = 0;
				hitPanzerRed = false;
				hitPanzerGreen = false;
			}
		}
		DrawLine(screenWidth / 2 - 5, screenHeigth / 2 - mixingUp, screenWidth / 2 - 1, screenHeigth / 2 - mixingUp, color);
		DrawLine(screenWidth / 2 + 1, screenHeigth / 2 - mixingUp, screenWidth / 2 + 5, screenHeigth / 2 - mixingUp, color);

		DrawLine(screenWidth / 2, screenHeigth / 2 - mixingUp - 1, screenWidth / 2, screenHeigth / 2 - mixingUp - 5, color);
		DrawLine(screenWidth / 2, screenHeigth / 2 - mixingUp + 1, screenWidth / 2, screenHeigth / 2 - mixingUp + 5, color);




	}
	void DrawBonusAim()
	{
		if (bonusAim == true)
		{
			interfaceSprite.setTextureRect(IntRect(1, 1, 40, 40));
			interfaceSprite.setPosition(250, 559);
			window.draw(interfaceSprite);
		}
	}
	void DrawBullet()// ���������� ���������� ����
	{
		for (int i = 1; i <= BULLET; i++)
		{
			DrawLine(299 + i * 3, 590, 299 + i * 3, 575, Color(255, 255, 0));
		}

	}
	void DrawTimeAttack()// ���������� ����� �����������
	{
		RectangleShape rectangle(sf::Vector2f((int)((float)CountAttack / (float)TimeAttack * 90), 5));
		rectangle.setPosition(302, 569);
		rectangle.setFillColor(Color(128, 255, 128));
		window.draw(rectangle);
	}
	void DrawHP()// ���������� ������ ��������
	{
		RectangleShape rectangle(sf::Vector2f((int)((float)HP / (float)100 * 90), 10));
		rectangle.setPosition(10, 579);
		rectangle.setFillColor(Color(255, 125, 128));
		window.draw(rectangle);
	}
	void DrawARMOR()// ���������� ������ �����
	{
		RectangleShape rectangle(sf::Vector2f((int)((float)BRONY / (float)100 * 90), 10));
		rectangle.setPosition(10, 569);
		rectangle.setFillColor(Color(128, 125, 255));
		window.draw(rectangle);
	}
	class Statistics
	{
		Image starsImage;
		Texture starsTexture;//������� ������ Texture (��������)
		Sprite starsSprite;
	public:
		Statistics()
		{
			starsImage.loadFromFile("Stars.png");//��������� � ���� ����
			starsImage.createMaskFromColor(Color(255, 255, 255));

			starsTexture.loadFromImage(starsImage);//�������� � ���� ������ Image (�����������)
			starsSprite.setTexture(starsTexture);
		}
		void DrawStarsPlayer(Text text)
		{
			if (starsPlayer < 16)text.setFillColor(Color(255, 255, 0)); else text.setFillColor(Color(255, 0, 0));
			std::ostringstream starsString;    // �������� ����������
			starsString << starsPlayer;

			text.setString(starsString.str());
			text.setPosition(450, 577);
			starsSprite.setTextureRect(IntRect(1, 1, 40, 40));
			starsSprite.setPosition(470, 563);
			window.draw(starsSprite);
			window.draw(text);
		}
		void DrawStarsBot(Text text)
		{
			if (starsBot < 16)text.setFillColor(Color(255, 255, 0)); else text.setFillColor(Color(255, 0, 0));
			std::ostringstream starsString;    // �������� ����������
			starsString << starsBot;

			text.setString(starsString.str());
			text.setPosition(663, 577);
			starsSprite.setTextureRect(IntRect(40, 1, 40, 40));
			starsSprite.setPosition(625, 563);
			window.draw(starsSprite);
			window.draw(text);
		}
		void DrawBoardScore(Text text)
		{
			text.setFillColor(Color(255, 255, 255));//��������� ����� � �������. ���� ������ ��� ������, �� �� ��������� �� �����
			std::ostringstream scoreString[2];    // �������� ����������
			scoreString[0] << scorePlayer;
			scoreString[1] << scoreBot;
			text.setString(scoreString[0].str() + " : " + scoreString[1].str());
			if (scorePlayer < 10) text.setPosition(550, 565); else text.setPosition(542, 565);
			window.draw(text);
			//	text.setStyle(sf::Text::Bold | sf::Text::Underlined);//������ � ������������ �����. �� ��������� �� "�����":)) � �� ������������

		}
		void Draw(Text text)
		{
			DrawBoardScore(text);
			DrawStarsPlayer(text);
			DrawStarsBot(text);

		}
	};
	Statistics statistics;

	void Draw()// ���������� ���������
	{
		DrawLine(1, 560, 800, 560, Color(128, 128, 128));
		DrawBullet();
		DrawTimeAttack();
		DrawHP();
		DrawARMOR();
		DrawBonusAim();
		DrawAim();
	}

};
GameInterface gameInterface;
void GrabBonus()// ������� ������� �������
{
	int valuePanzer = -1,// ����� ������������ ������ �������
		valuePanzerBot = -1;// ����� ������������ ������ �����
	if (panzer.Get_Being() == true)
	{
		valuePanzer = bonuses.CrossBonus(panzer.Get_X(), panzer.Get_Y(), panzer.Get_Size() * 2, panzer.Get_Size() * 2);
	}
	if (panzerBot.Get_Being() == true)
	{
		valuePanzerBot = bonuses.CrossBonus(panzerBot.Get_X(), panzerBot.Get_Y(),
			panzerBot.Get_Size() * 2, panzerBot.Get_Size() * 2);
	}
	if (valuePanzer != -1)
	{
		bonuses.KillBonus(valuePanzer);
		if (bonuses.bonus[valuePanzer].type == 1) panzer.Add_Pylu(5);// 1- ����� ����
		if (bonuses.bonus[valuePanzer].type == 2)// 2- ����� ���������
		{
			panzer.Add_TimeAttack(-3);
			starsPlayer++;
		}
		if (bonuses.bonus[valuePanzer].type == 3) panzer.Add_Brony(100);// 3-����� �����
		if (bonuses.bonus[valuePanzer].type == 4)
		{
			panzer.speedTurn = true;// 3-����� �����

		}
	}
	if (valuePanzerBot != -1)
	{
		bonuses.KillBonus(valuePanzerBot);
		if (bonuses.bonus[valuePanzerBot].type == 1) panzerBot.Add_Pylu(5);
		if (bonuses.bonus[valuePanzerBot].type == 2)
		{
			panzerBot.Add_TimeAttack(-3);
			starsBot++;
			// panzerBot.CancelMovingToPuty();
		}
		if (bonuses.bonus[valuePanzerBot].type == 3)
		{
			panzerBot.Add_Brony(100);
			// panzerBot.CancelMovingToPuty();
		}
	}
}
bool BulletInPanzer()
{
	for (int i = 0; i < amountBullet; i++)
		if (bullets[i].being == true)
			if (panzer.CrossMe(bullets[i].x, bullets[i].y))// ���� � ������ ������ ����
			{
				bullets[i].being = false;
				//	if (panzer.Get_Brony()>=25)
				//������ �������� � �����
				if (panzer.Get_Brony() > 0)
				{
					panzer.Add_Brony(-25);
					if (panzer.Get_Brony() == 0)
					{
						bullets[i].being = false;
						if (networkGame == false || (networkGame == true && mode == 's'))
						{
							shakesCamera = true;

						}
						hitPanzerGreen = true;
						break;
					}
				}
				if (panzer.Get_Brony() <= 0)
				{
					panzer.Add_HP(-25);
				}
				if (panzer.Get_HP() <= 0)	// ���� �������� ���
				{

					return true;
				}
				else
				{
					if (networkGame == false || (networkGame == true && mode == 's'))
					{
						shakesCamera = true;

					}
					hitPanzerGreen = true;
					burstes.Registration(bullets[i].x, bullets[i].y, true);
				}
			}
	return false;
}
bool PyleInPanzerBot()
{
	for (int i = 0; i < amountBullet; i++)
		if (bullets[i].being == true)
			if (panzerBot.CrossMe(bullets[i].x, bullets[i].y))// ���� � ������ ������ ����
			{
				//if (panzer.Get_Brony()>=25)
				//������ �������� � �����
				bullets[i].being = false;
				if (panzerBot.Get_Brony() > 0)
				{
					panzerBot.Add_Brony(-25);
					if (panzerBot.Get_Brony() == 0)
					{
						if (networkGame == true && mode == 'c')
						{
							shakesCamera = true;

						}
						bullets[i].being = false;
						hitPanzerRed = true;
						break;
					}
				}
				if (panzerBot.Get_Brony() <= 0)
				{
					panzerBot.Add_HP(-25);

				}
				if (panzerBot.Get_HP() <= 0)	// ���� �������� ���
				{
					return true;
				}
				else
				{
					if (networkGame == true && mode == 'c')
					{
						shakesCamera = true;

					}
					hitPanzerRed = true;
					burstes.Registration(bullets[i].x, bullets[i].y, true);
				}
			}
	return false;
}
Vector2i CalcPanzerCoordinate(bool killPanzer, bool killPanzerBot)
{
	bool notLocking = false;
	bool notWalls = false;
	int x = 0, y = 0;
	Vector2i result;
	result.x = 0;
	result.y = 0;
	if (killPanzer || killPanzerBot)// ���� ���� ����� ��� ��� �� ��������� ������ ����������
	{

		do
		{
			notLocking = false;
			notWalls = false;
			x = (rand() % 740) + 30;
			y = (rand() % 500) + 30;
			// ���� � � � �� �������� � ����� 
			if (walls.CrossWall(x, y, panzer.Get_Size() * 2, panzer.Get_Size() * 2) == false) notWalls = true;

			{
				//���� � � � �� �������� ��� ���� �������� �����
				if (killPanzer)
				{
					if (LookAcrossWall(x + panzer.Get_Size(), y + panzer.Get_Size(),
						panzerBot.Get_X() + panzerBot.Get_Size(),
						panzerBot.Get_Y() + panzerBot.Get_Size()) == false)
					{
						notLocking = true;

					}
				}
				if (killPanzerBot)
				{
					if (LookAcrossWall(x + panzerBot.Get_Size(), y + panzerBot.Get_Size(),
						panzer.Get_X() + panzer.Get_Size(),
						panzer.Get_Y() + panzer.Get_Size()) == false)
					{
						notLocking = true;

					}
				}
			}
			// ���� � � � �� �������� � ����� , �� ��������� � ���� ��������� �����, � �� �������� �� �����
		} while ((notWalls == false || notLocking == false)
			|| bonuses.CrossBonus(x - panzer.Get_Size(), y - panzer.Get_Size(),
				panzer.Get_Size() * 3, panzer.Get_Size() * 3) != -1
			|| bonuses.CrossBonus(x - panzerBot.Get_Size(), y - panzerBot.Get_Size(),
				panzerBot.Get_Size() * 3, panzerBot.Get_Size() * 3) != -1);

	}
	result.x = x;
	result.y = y;
	return result;
}


void NewCoordinateKill()// ����� ���������� ����� ������ �����
{
	bool killPanzer = false, killPanzerBot = false, flagPanzer = false, flagPanzerBot = false;
	int oldPanzerX = 0, oldPanzerY = 0, oldPanzerBotX = 0, oldPanzerBotY = 0;
	flagPanzer = BulletInPanzer();
	if (flagPanzer == true)	// ���� �������� ��� � �������� �����
	{
		if (networkGame == false)
		{
			killPanzer = true;
			panzer.KIlled();
			// ����������� ������
			burstes.Registration(panzer.Get_X() + panzer.Get_Size(),
				panzer.Get_Y() + panzer.Get_Size());
			// ����������� ������
			/*bonuses.NewBonus(panzer.Get_X()+panzer.Get_Size(),
			panzer.Get_Y()+panzer.Get_Size(),(rand()%2)+2,true);*/
			oldPanzerX = panzer.Get_X();
			oldPanzerY = panzer.Get_Y();
			scoreBot++;
		}
		if (networkGame == true)
		{
			if (mode == 's')
			{
				killPanzer = true;
				panzer.KIlled();
				// ����������� ������
				burstes.Registration(panzer.Get_X() + panzer.Get_Size(),
					panzer.Get_Y() + panzer.Get_Size());
				// ����������� ������
				/*bonuses.NewBonus(panzer.Get_X()+panzer.Get_Size(),
				panzer.Get_Y()+panzer.Get_Size(),(rand()%2)+2,true);*/
				oldPanzerX = panzer.Get_X();
				oldPanzerY = panzer.Get_Y();
				scoreBot++;
				server.Send(6, 0, 0, 1, 0);
			}
		}

	}
	flagPanzerBot = PyleInPanzerBot();
	if (flagPanzerBot == true)
	{
		if (networkGame == false)
		{
			killPanzerBot = true;
			panzerBot.KIlled();
			burstes.Registration(panzerBot.Get_X() + panzerBot.Get_Size(),
				panzerBot.Get_Y() + panzerBot.Get_Size());
			oldPanzerBotX = panzerBot.Get_X();
			oldPanzerBotY = panzerBot.Get_Y();
			scorePlayer++;
		}
		if (networkGame == true)
			if (mode == 'c')
			{
				killPanzerBot = true;// ������� ������� ���� �� �������
				panzerBot.KIlled();
				burstes.Registration(panzerBot.Get_X() + panzerBot.Get_Size(),
					panzerBot.Get_Y() + panzerBot.Get_Size());
				scorePlayer++;
				client.Send(6, 0, 0, 2, 0);// �������� ������� ���� � ������ �������� �����
			}

	}

	if (networkGame == true)// ������������ �������� ������
	{
		if (mode == 's')// �� �������
		{
			Data data;
			for (int i = 0; i < countPacked; i++)
			{
				data = dataServerRace[i];
				if (data.key == 6 && data.tip == 2)// ���� �����l� �� ������� ���� � ������ �������� �����
				{
					oldPanzerBotX = panzerBot.Get_X();// ��������� ������ ����������
					oldPanzerBotY = panzerBot.Get_Y();
					killPanzerBot = true;
					panzerBot.KIlled();// ������� ������� ���� �� �������
					scorePlayer++;
					burstes.Registration(panzerBot.Get_X() + panzerBot.Get_Size(),
						panzerBot.Get_Y() + panzerBot.Get_Size());
					dataServerRace[i].key = 0;
				}
			}

		}
		if (mode == 'c')// �� �������
		{
			Data data;
			for (int i = 0; i < countPacked; i++)
			{
				data = dataClientRace[i];
				if (data.key == 6 && data.tip == 1)// ���� �����l� �� ������� ���� � ������ �������� �����
				{
					killPanzer = true;
					panzer.KIlled();// ������� ������� ���� �� �������
					burstes.Registration(panzer.Get_X() + panzer.Get_Size(),
						panzer.Get_Y() + panzer.Get_Size());
					scoreBot++;
					dataClientRace[i].key = 0;
				}
			}
		}
	}
	int x = 0, y = 0;
	Vector2i newCoord;
	if (killPanzer || killPanzerBot)
	{
		newCoord = CalcPanzerCoordinate(killPanzer, killPanzerBot);
		x = newCoord.x;
		y = newCoord.y;
	}


	if (killPanzer)// ���� ���� ������ ��� ���� 
	{
		// �� ��������� ��� ����� ��������� � �������� ���������
		killPanzer = false;
		panzer.Put_Pylu(5);
		panzer.Put_HP(100);

		panzer.speedTurn = false;
		if (networkGame == false)
		{
			bonuses.NewBonus(oldPanzerX + panzer.Get_Size(),
				oldPanzerY + panzer.Get_Size(), (rand() % 2) + 2, true);
			panzer.Put_X(x);
			panzer.Put_Y(y);
		}
		// ������� ���� 
		if (networkGame == true)
		{
			if (mode == 's')
			{
				int oldX = oldPanzerX + panzer.Get_Size();
				int oldY = oldPanzerY + panzer.Get_Size();
				int rTip = (rand() % 2) + 2;
				bonuses.NewBonus(oldX, oldY, rTip, true);
				server.Send(7, x, y, 1, 0);
				server.Send(2, oldX, oldY, rTip, 0);
				//bonuses.NewBonus(oldPanzerX+panzer.Get_Size(),
				//					oldPanzerY+panzer.Get_Size(),(rand()%2)+2,true);
				panzer.Put_X(x);
				panzer.Put_Y(y);

				//	server.Send(3,x,y,0,0);
			}
		}
	}
	// ����������
	if (killPanzerBot)// ���� ����� ������� ����
	{
		killPanzerBot = false;
		panzerBot.Put_Pylu(5);
		panzerBot.Put_HP(100);

		if (networkGame == false)// ���� ������ ����
		{
			bonuses.NewBonus(oldPanzerBotX + panzerBot.Get_Size(),
				oldPanzerBotY + panzerBot.Get_Size(), (rand() % 2) + 2, true);
			panzerBot.Put_X(x);
			panzerBot.Put_Y(y);
		}
		if (networkGame == true)// ���� ������� ����
		{
			if (mode == 's')// �� �������
			{
				int oldX = oldPanzerBotX + panzerBot.Get_Size();
				int oldY = oldPanzerBotY + panzerBot.Get_Size();
				int rTip = (rand() % 2) + 2;
				bonuses.NewBonus(oldX, oldY, rTip, true);
				server.Send(2, oldX, oldY, rTip, 0);
				panzerBot.Put_X(x);
				panzerBot.Put_Y(y);
				server.Send(7, x, y, 2, 0);
			}

		}
		// ������� ����

		if (panzerBot.motionToPuty == true)
		{
			panzerBot.BonusControl(true);
		}
	}
	if (networkGame == true)
	{
		if (mode == 'c')
		{
			Data data;
			for (int i = 0; i < countPacked; i++)
			{
				data = dataClientRace[i];
				if (data.key == 7 && data.tip == 2)
				{
					panzerBot.Put_X((int)data.x);
					panzerBot.Put_Y((int)data.y);
				}
				if (data.key == 7 && data.tip == 1)
				{
					panzer.Put_X((int)data.x);
					panzer.Put_Y((int)data.y);
				}
			}
		}
	}
}


void DrawLine(int x, int y, int x1, int y1, Color color, RenderWindow& window)// ��������� ��������� �����
{
	//������� ������ �����, �� ������� ����� ���������� �����:
	sf::VertexArray lines(sf::Lines, 2/*���������� �����*/);

	//����� ��� ������ �� ����� ������ ���� �������:
	lines[0].position = sf::Vector2f(x, y);
	lines[1].position = sf::Vector2f(x1, y1);
	//� �.�.

	//����� ��� ������ ����� ��������� ����(��� ����� ��������� ���������):
	lines[0].color = color;
	lines[1].color = color;
	//� �.�.

	//� � ����� ������� ��� �� �����:
	window.draw(lines);
}
const int amountLines = 51;
class Camera
{

public:

	Vector2f point;
	enum Type// ���� ��������
	{
		EMPTY, WALL, PANZRED, PANZGREEN, BONUSBULLETS, BONUSARMOUR, BONUSSTAR
	};
	struct DataLine// ������ �� �������� ����
	{
		Vector2f begin;
		Vector2f end;
	};

	struct Data// ������ � ������, ������� �����������������
	{
		float dist;// ���������
		Vector2f acrossPoint;// ����� �����������
		Color color;
		Type type;
	};
	DataLine dataLine[amountWalls * 4];// ������ ������ � ������
	DataLine dataLineBonus[amountBonus * 4];// ������ ������ � ������
	DataLine dataLinePanzerGreen[4];
	DataLine dataLinePanzerRed[4];
	Data data[amountLines];// ������ ����� ���������

	float dir;// ����������
	float FieldOfView;// ���� ������� ��������� � ���
	float StepOfField;// ���� ����� ������
	float WalkSpeed;// �������� ������
	float ViewSpeed;//�������� ������ �����
					//float MaxDistance;// ������������ ��������� ���������

	void BlockToLines()// ������� ����������� � ����������� ����� ������ � ����� ����
	{

		for (int i = 0; i < amountWalls; i++)
		{
			Vector2f A = Vector2f(walls.wall[i].x, walls.wall[i].y);
			Vector2f B = Vector2f(walls.wall[i].x + walls.size, walls.wall[i].y);
			Vector2f C = Vector2f(walls.wall[i].x + walls.size, walls.wall[i].y + walls.size);
			Vector2f D = Vector2f(walls.wall[i].x, walls.wall[i].y + walls.size);

			dataLine[i * 4 + 0].begin = A;
			dataLine[i * 4 + 0].end = B;

			dataLine[i * 4 + 1].begin = B;
			dataLine[i * 4 + 1].end = C;

			dataLine[i * 4 + 2].begin = C;
			dataLine[i * 4 + 2].end = D;

			dataLine[i * 4 + 3].begin = D;
			dataLine[i * 4 + 3].end = A;
		}

	}
	void LinePanzerGreen()
	{
		float mult = 1.87;
		Vector2f A = Vector2f(panzer.Get_X(), panzer.Get_Y());
		Vector2f B = Vector2f(panzer.Get_X() + panzer.Get_Size() * mult, panzer.Get_Y());
		Vector2f C = Vector2f(panzer.Get_X() + panzer.Get_Size() * mult, panzer.Get_Y() + panzer.Get_Size() * mult);
		Vector2f D = Vector2f(panzer.Get_X(), panzer.Get_Y() + panzer.Get_Size() * mult);
		dataLinePanzerGreen[0].begin = A;
		dataLinePanzerGreen[0].end = B;

		dataLinePanzerGreen[1].begin = B;
		dataLinePanzerGreen[1].end = C;

		dataLinePanzerGreen[2].begin = C;
		dataLinePanzerGreen[2].end = D;

		dataLinePanzerGreen[3].begin = D;
		dataLinePanzerGreen[3].end = A;
	}
	void LinePanzerRed()
	{
		float mult = 1.87;
		Vector2f A = Vector2f(panzerBot.Get_X(), panzerBot.Get_Y());
		Vector2f B = Vector2f(panzerBot.Get_X() + (float)panzerBot.Get_Size() * mult, panzerBot.Get_Y());
		Vector2f C = Vector2f(panzerBot.Get_X() + (float)panzerBot.Get_Size() * mult, panzerBot.Get_Y() + (float)panzerBot.Get_Size() * mult);
		Vector2f D = Vector2f(panzerBot.Get_X(), panzerBot.Get_Y() + (float)panzerBot.Get_Size() * mult);
		dataLinePanzerRed[0].begin = A;
		dataLinePanzerRed[0].end = B;

		dataLinePanzerRed[1].begin = B;
		dataLinePanzerRed[1].end = C;

		dataLinePanzerRed[2].begin = C;
		dataLinePanzerRed[2].end = D;

		dataLinePanzerRed[3].begin = D;
		dataLinePanzerRed[3].end = A;
	}
	Camera()
	{
		point.x = screenWidth / 2;
		point.y = screenHeigth / 2;
		dir = 2.5;
		FieldOfView = 1;
		StepOfField = 0.02;
		WalkSpeed = 0.04;
		ViewSpeed = 0.002f;
		//float dir;// ����������
		//float FieldOfView;// ���� ������� ��������� � ���
		//float StepOfField;// ���� ����� ������
		//float WalkSpeed;// �������� ������
		//float ViewSpeed;//�������� ������ �����
		//	MaxDistance = 500;
		//	BlockToLines();
	}
	void BonusToLine()
	{

		for (int i = 0; i < amountBonus; i++)
		{

			//if (bonuses.bonus[i].being == true)
			{
				Vector2f A = Vector2f(bonuses.bonus[i].x, bonuses.bonus[i].y);
				Vector2f B = Vector2f(bonuses.bonus[i].x + bonuses.size, bonuses.bonus[i].y);
				Vector2f C = Vector2f(bonuses.bonus[i].x + bonuses.size, bonuses.bonus[i].y + bonuses.size);
				Vector2f D = Vector2f(bonuses.bonus[i].x, bonuses.bonus[i].y + bonuses.size);

				dataLineBonus[i * 4 + 0].begin = A;
				dataLineBonus[i * 4 + 0].end = B;

				dataLineBonus[i * 4 + 1].begin = B;
				dataLineBonus[i * 4 + 1].end = C;

				dataLineBonus[i * 4 + 2].begin = C;
				dataLineBonus[i * 4 + 2].end = D;

				dataLineBonus[i * 4 + 3].begin = D;
				dataLineBonus[i * 4 + 3].end = A;

			}
			if (Keyboard::isKeyPressed(Keyboard::B))
			{
				int a;
//				a = a;
			}
		}
	}
	float distance(float x, float y, float x1, float y1)
	{
		float dx = abs(x1 - x),
			dy = abs(y1 - y);
		return sqrt(dx * dx + dy * dy);
	}
	float VectMult(float ax, float ay, float bx, float by)
	{
		return (ax * by) - (ay * bx);
	}

	bool IsCrossing(float a1x, float a1y, float a2x, float a2y, float b1x, float b1y, float b2x, float b2y)
	{
		float v1 = VectMult(b2x - b1x, b2y - b1y, a1x - b1x, a1y - b1y);
		float v2 = VectMult(b2x - b1x, b2y - b1y, a2x - b1x, a2y - b1y);
		float v3 = VectMult(a2x - a1x, a2y - a1y, b1x - a1x, b1y - a1y);
		float v4 = VectMult(a2x - a1x, a2y - a1y, b2x - a1x, b2y - a1y);
		return (v1 * v2) <= 0 && (v3 * v4) <= 0;
	}

	Vector2f CrossingPoint(float a1, float b1, float c1, float a2, float b2, float c2)
	{
		float d = a1 * b2 - b1 * a2;
		float dx = -c1 * b2 + b1 * c2;
		float dy = -a1 * c2 + c1 * a2;
		return Vector2f(dx / d, dy / d);
	}

	Vector2f GetCrossVector(Vector2f aP1, Vector2f aP2, Vector2f bP1, Vector2f bP2)
	{
		float a1, b1, c1, a2, b2, c2;
		a1 = aP2.y - aP1.y;
		b1 = aP1.x - aP2.x;
		c1 = -aP1.x * (aP2.y - aP1.y) + aP1.y * (aP2.x - aP1.x);
		a2 = bP2.y - bP1.y;
		b2 = bP1.x - bP2.x;
		c2 = -bP1.x * (bP2.y - bP1.y) + bP1.y * (bP2.x - bP1.x);
		return CrossingPoint(a1, b1, c1, a2, b2, c2);
	}

	void Control(RenderWindow& window)// ������� ���������� � ����������
	{
		float speed = 1.5;
		if (Keyboard::isKeyPressed(Keyboard::W))// ���� ������ 
		{
			point.x += sin(dir) * speed;
			point.y += cos(dir) * speed;
		}
		if (Keyboard::isKeyPressed(Keyboard::S))// ���� ����� 
		{
			point.x -= sin(dir) * speed;
			point.y -= cos(dir) * speed;

		}
		if (Keyboard::isKeyPressed(Keyboard::A))// ������� ������ ������� 
		{
			point.x -= sin(dir - 1.57) * speed;
			point.y -= cos(dir - 1.57) * speed;
		}
		if (Keyboard::isKeyPressed(Keyboard::D))// ������� �� �������
		{
			point.x -= sin(dir + 1.57) * speed;
			point.y -= cos(dir + 1.57) * speed;
		}
	}
	//void ControlMouse()
	//{
	//	static int flagBeginProg = false;
	//	if (flagBeginProg == false)
	//	{
	//		flagBeginProg = true;
	//		window.setMouseCursorVisible(false);
	//	}
	//	if (Keyboard::isKeyPressed(Keyboard::M))// ������� �� �������
	//	{
	//		mouseCapture = !mouseCapture;
	//		//window.setMouseCursorVisible() // �������� ������
	//		window.setMouseCursorVisible(!mouseCapture);


	//		while (Keyboard::isKeyPressed(Keyboard::M))
	//		{

	//		}
	//	}

	//	if (mouseCapture == true)
	//	{
	//		static Vector2i oldMouse = Mouse::getPosition(window);
	//		Vector2i mousePos = Mouse::getPosition(window);//�������� ���������� �������
	//		Vector2i winPos = window.getPosition();
	//		if (oldMouse.x > mousePos.x)
	//		{
	//			dir += (oldMouse.x - mousePos.x) / (float)500;
	//		}
	//		else if (oldMouse.x < mousePos.x)
	//		{
	//			dir -= (mousePos.x - oldMouse.x) / (float)500;
	//		}
	//		Mouse::setPosition(Vector2i(winPos.x + screenWidth / 2, winPos.y + screenHeigth / 2));// ���������� ������� � ���������������� ����������  
	//		oldMouse = Mouse::getPosition(window);
	//	}
	//}
	void ShakeCamera()
	{
		static int count = 0;
		int maxCount = 20;
		static int oldX = -1, oldY = -1;
		if (shakesCamera == true)
		{
			if (oldX == -1 && oldY == -1)
			{
				oldX = point.x;
				oldY = point.y;
			}
			if (count < maxCount)
			{
				point.x = oldX + 3 - rand() % 7;
				point.y = oldY + 3 - rand() % 7;
				count++;
			}
			else
			{
				oldX = oldY = -1;
				shakesCamera = false;
				count = 0;
			}
		}
	}
	void Services(typePanzer tpPanz, RenderWindow& window)// ������� ������� ���������� � ���� ���� ��������� ����� ����
	{
		//Control(window);
		static float dirOld = 0;
		dir = ControlMouse();
		if (isnan(dir) != 0)
		{
			dir = dirOld;
		}
		if (Keyboard::isKeyPressed(Keyboard::C))
		{
			shakesCamera = true;
		}
		if (shakesCamera == false)
		{
			if (tpPanz == GREEN)
			{
				point.x = panzer.Get_X() + panzer.Get_Size();
				point.y = panzer.Get_Y() + panzer.Get_Size();

			}
			else if (tpPanz == RED)
			{
				point.x = panzerBot.Get_X() + panzerBot.Get_Size();
				point.y = panzerBot.Get_Y() + panzerBot.Get_Size();

			}
		}
		ShakeCamera();
		BonusToLine();
		bool reyBotCenter = false;
		float st = -FieldOfView / 2;// ����������� ��������� ���� ������ ��� ������ �����
		for (int i = 0; i < amountLines; i++)
		{
			// ����������� ����� ����������� � ����� ������� ������� ����� ����
			data[i].type = EMPTY;
			data[i].acrossPoint = Vector2f(
				(float)(MaxDistance * sin(dir + st) + point.x),
				(float)(MaxDistance * cos(dir + st) + point.y));
			for (int j = 0; j < amountWalls * 4; j++)
			{

				if (data[i].type == EMPTY)
				{
					// ��������� ���� ����������� �� ������
					if (IsCrossing(point.x, point.y, data[i].acrossPoint.x, data[i].acrossPoint.y,
						dataLine[j].begin.x, dataLine[j].begin.y, dataLine[j].end.x, dataLine[j].end.y))
					{
						// �������������� ����� ������������
						data[i].acrossPoint = GetCrossVector(point, data[i].acrossPoint, dataLine[j].begin, dataLine[j].end);


						//���� ����� ����������� ����� � ����� ����� �����
						//if (distance(data[i].acrossPoint.x, data[i].acrossPoint.y, dataLine[j].begin.x, dataLine[j].begin.y) <0||
						//	distance(data[i].acrossPoint.x, data[i].acrossPoint.y, dataLine[j].end.x, dataLine[j].end.y) < 0)
							data[i].color = Color::White;
						//else
							//data[i].color = Color::Green;

					}
					//data[i].type = WALL;
					// ����������� ��������� �� ������ �� ����� ����������� �� ������
					data[i].dist = distance(point.x, point.y, data[i].acrossPoint.x, data[i].acrossPoint.y);
					// �������� �� ������� ���� ����� ����������� �� ������ ������������
					if (data[i].dist < MaxDistance - 0.1)
					{
						data[i].dist *= cos(st);
					}
				}

			}
			if (tpPanz == GREEN)
			{
				for (int j = 0; j < 4; j++)
				{
					if (IsCrossing(point.x, point.y, data[i].acrossPoint.x, data[i].acrossPoint.y,
						dataLinePanzerRed[j].begin.x, dataLinePanzerRed[j].begin.y, dataLinePanzerRed[j].end.x, dataLinePanzerRed[j].end.y))
					{
						Data oneData;
						oneData.acrossPoint = GetCrossVector(point, data[i].acrossPoint, dataLinePanzerRed[j].begin, dataLinePanzerRed[j].end);
						data[i].acrossPoint = oneData.acrossPoint;
						data[i].type = PANZRED;
					}
					data[i].dist = distance(point.x, point.y, data[i].acrossPoint.x, data[i].acrossPoint.y);
					// �������� �� ������� ���� ����� ����������� �� ������ ������������
					if (data[i].dist < MaxDistance - 0.1)
					{
						data[i].dist *= cos(st);
					}

				}
			}
			if (tpPanz == RED)
			{
				for (int j = 0; j < 4; j++)
				{
					if (IsCrossing(point.x, point.y, data[i].acrossPoint.x, data[i].acrossPoint.y,
						dataLinePanzerGreen[j].begin.x, dataLinePanzerGreen[j].begin.y, dataLinePanzerGreen[j].end.x, dataLinePanzerGreen[j].end.y))
					{
						Data oneData;
						oneData.acrossPoint = GetCrossVector(point, data[i].acrossPoint, dataLinePanzerGreen[j].begin, dataLinePanzerGreen[j].end);
						data[i].acrossPoint = oneData.acrossPoint;
						data[i].type = PANZGREEN;
					}
					data[i].dist = distance(point.x, point.y, data[i].acrossPoint.x, data[i].acrossPoint.y);
					// �������� �� ������� ���� ����� ����������� �� ������ ������������
					if (data[i].dist < MaxDistance - 0.1)
					{
						data[i].dist *= cos(st);
					}

				}


				data[i].dist = distance(point.x, point.y, data[i].acrossPoint.x, data[i].acrossPoint.y);
				// �������� �� ������� ���� ����� ����������� �� ������ ������������
				if (data[i].dist < MaxDistance - 0.1)
				{
					data[i].dist *= cos(st);
				}

			}
			for (int j = 0; j < amountBonus * 4; j++)
			{
				if (bonuses.bonus[j / 4].being == true)
				{
					if (IsCrossing(point.x, point.y, data[i].acrossPoint.x, data[i].acrossPoint.y,
						dataLineBonus[j].begin.x, dataLineBonus[j].begin.y, dataLineBonus[j].end.x, dataLineBonus[j].end.y))
					{
						Data oneData;
						oneData.acrossPoint = GetCrossVector(point, data[i].acrossPoint, dataLineBonus[j].begin, dataLineBonus[j].end);
						data[i].acrossPoint = oneData.acrossPoint;
						/*switch (bonuses.bonus[j].type)
						{
						case 1:  data[i].type = BONUSBULLETS; break;
						case 2:  data[i].type = BONUSSTAR; break;
						case 3:  data[i].type = BONUSARMOUR; break;

						}*/
						if (bonuses.bonus[j / 4].type == 1) data[i].type = BONUSBULLETS;
						if (bonuses.bonus[j / 4].type == 2) data[i].type = BONUSSTAR;
						if (bonuses.bonus[j / 4].type == 3) data[i].type = BONUSARMOUR;
					}
				}
				data[i].dist = distance(point.x, point.y, data[i].acrossPoint.x, data[i].acrossPoint.y);
				// �������� �� ������� ���� ����� ����������� �� ������ ������������
				if (data[i].dist < MaxDistance - 0.1)
				{
					data[i].dist *= cos(st);
				}
			}

			st += StepOfField;// �������� ���������� ���
		}


		dirOld == dir;
	}

	void Draw(RenderWindow& window)// ������� ���������
	{
		// ������ ������� ���
		RectangleShape rectangle(Vector2f(screenWidth, screenHeigth / 2));
		CircleShape shape;
		rectangle.setFillColor(Color(128, 128, 255));
		RectangleShape rectEdge(Vector2f(3, 3));
		rectEdge.setFillColor(Color(255, 128, 0));
		rectangle.setPosition(1, 1);
		window.draw(rectangle);
		// ������ ������ ���
		rectangle.setFillColor(Color(255, 128, 128));
		rectangle.setPosition(1, screenHeigth / 2 + 1 - 40);
		window.draw(rectangle);
		RectangleShape rect3D;
		float multHeight = 0.15;
		int widthRect3D = 20;
		for (int i = 0; i < amountLines; i++)
		{
			float dist = data[i].dist;// *cos((pi));
			if (dist < MaxDistance - 0.1)
			{

				float lineHeight = dist * multHeight;
				if (data[i].color == Color::White)// ������ ����� ����� ���� ��� �� ���� ������� �����
				{
					rect3D.setFillColor(Color(200 - lineHeight * 1.5, 200 - lineHeight * 1.5, 200 - lineHeight * 1.5));
					rect3D.setSize(Vector2f(widthRect3D, 1000 / lineHeight * 2));
					rect3D.setPosition((int)(screenWidth / (float)amountLines * (amountLines - i)),
						screenHeigth / 2 - 40 - 1000 / lineHeight);

					/*DrawLine((int)(screenWidth / (float)amountLines * (amountLines - i)), screenHeigth / 2-40 - 1000 / lineHeight,
					(int)(screenWidth / (float)amountLines * (amountLines - i)), screenHeigth / 2-40 + 1000 / lineHeight,
					Color(200 - lineHeight / 2, 200 - lineHeight / 2, 200 - lineHeight / 2), window);*/
					window.draw(rect3D);
				}
				else
				{	// ������ ������� ����� ���� ��� ���� �����
					rect3D.setFillColor(Color(50, 150 - lineHeight / 2, 50));
					rect3D.setSize(Vector2f(widthRect3D, 1000 / lineHeight * 2));
					rect3D.setPosition((int)(screenWidth / (float)amountLines * (amountLines - i)),
						screenHeigth / 2 - 40 - 1000 / lineHeight);
					window.draw(rect3D);
					/*DrawLine((int)(screenWidth / (float)amountLines * (amountLines - i)), screenHeigth / 2-40 - 1000 / lineHeight,
					(int)(screenWidth / (float)amountLines * (amountLines - i)), screenHeigth / 2-40 + 1000 / lineHeight,
					Color::White, window);*/
				}

			}
			if (data[i].type == PANZRED)
			{
				float lineHeight = dist * multHeight;
				rect3D.setFillColor(Color::Red);
				rect3D.setSize(Vector2f(widthRect3D, 1000 / lineHeight * 2));
				rect3D.setPosition((int)(screenWidth / (float)amountLines * (amountLines - i)),
					screenHeigth / 2 - 40 - 1000 / lineHeight);
				window.draw(rect3D);
			}
			if (data[i].type == PANZGREEN)
			{
				float lineHeight = dist * multHeight;
				rect3D.setFillColor(Color::Green);
				rect3D.setSize(Vector2f(widthRect3D, 1000 / lineHeight * 2));
				rect3D.setPosition((int)(screenWidth / (float)amountLines * (amountLines - i)),
					screenHeigth / 2 - 40 - 1000 / lineHeight);
				window.draw(rect3D);
			}
			if (data[i].type == BONUSBULLETS)
			{
				float lineHeight = dist * multHeight;
				rect3D.setFillColor(Color(150, 75, 0));
				rect3D.setSize(Vector2f(widthRect3D, 1000 / lineHeight * 2));
				rect3D.setPosition((int)(screenWidth / (float)amountLines * (amountLines - i)),
					screenHeigth / 2 - 40 - 1000 / lineHeight);
				window.draw(rect3D);
			}
			if (data[i].type == BONUSARMOUR)
			{
				float lineHeight = dist * multHeight;
				rect3D.setFillColor(Color::Blue);
				rect3D.setSize(Vector2f(widthRect3D, 1000 / lineHeight * 2));
				rect3D.setPosition((int)(screenWidth / (float)amountLines * (amountLines - i)),
					screenHeigth / 2 - 40 - 1000 / lineHeight);
				window.draw(rect3D);
			}
			if (data[i].type == BONUSSTAR)
			{
				float lineHeight = dist * multHeight;
				rect3D.setFillColor(Color(255, 128, 0));
				rect3D.setSize(Vector2f(widthRect3D, 1000 / lineHeight * 2));
				rect3D.setPosition((int)(screenWidth / (float)amountLines * (amountLines - i)),
					screenHeigth / 2 - 40 - 1000 / lineHeight);
				window.draw(rect3D);
			}
			if (i <= amountLines / 2 + 15 && i >= amountLines / 2 - 15)
			{
				// ������ ����� �� ����� ������������ ���� ������� �����
				DrawLine(point.x / 4, point.y / 4, data[i].acrossPoint.x / 4, data[i].acrossPoint.y / 4, Color::Red, window);
			}
			else if (data[i].type == PANZRED)
			{
				DrawLine(point.x / 4, point.y / 4, data[i].acrossPoint.x / 4, data[i].acrossPoint.y / 4, Color::Blue, window);
			}
			else
			{
				// ������ ������ ����� ��� ����� �����
				DrawLine(point.x / 4, point.y / 4, data[i].acrossPoint.x / 4, data[i].acrossPoint.y / 4, Color::Yellow, window);
			}

		}
		RectangleShape recWall(Vector2f(walls.size / 4, walls.size / 4));
		rectangle.setFillColor(Color(128, 128, 128));
		for (int i = 0; i < amountWalls; i++)
		{
			recWall.setPosition(walls.wall[i].x / 4, walls.wall[i].y / 4);
			window.draw(recWall);
		}
		for (int i = 0; i < amountWalls * 4; i++)
		{
			DrawLine(dataLine[i].begin.x / 4, dataLine[i].begin.y / 4,
				dataLine[i].end.x / 4, dataLine[i].end.y / 4, Color::Blue, window);
		}
		// ������ ����� ������� ��������
		Vector2f line1 = point, line2 = point;
		float realDir = dir;
		realDir = realDir * 180 / pi;

		line1.y = 120 * cos(pi * (realDir + 90) / (float)180) + point.y;
		line1.x = 120 * sin(pi * (realDir + 90) / (float)180) + point.x;

		line2.y = 120 * cos(pi * (realDir - 90) / (float)180) + point.y;
		line2.x = 120 * sin(pi * (realDir - 90) / (float)180) + point.x;
		DrawLine(point.x / 4, point.y / 4, line1.x / 4, line1.y / 4, Color::Green, window);
		DrawLine(point.x / 4, point.y / 4, line2.x / 4, line2.y / 4, Color::Green, window);
	}
	void DrawLinePanzerREd(RenderWindow& window)
	{
		DrawLine(dataLinePanzerRed[0].begin.x / 4, dataLinePanzerRed[0].begin.y / 4,
			dataLinePanzerRed[0].end.x / 4, dataLinePanzerRed[0].end.y / 4, Color::Red, window);

		DrawLine(dataLinePanzerRed[1].begin.x / 4, dataLinePanzerRed[1].begin.y / 4,
			dataLinePanzerRed[1].end.x / 4, dataLinePanzerRed[1].end.y / 4, Color::Red, window);

		DrawLine(dataLinePanzerRed[2].begin.x / 4, dataLinePanzerRed[2].begin.y / 4,
			dataLinePanzerRed[2].end.x / 4, dataLinePanzerRed[2].end.y / 4, Color::Red, window);

		DrawLine(dataLinePanzerRed[3].begin.x / 4, dataLinePanzerRed[3].begin.y / 4,
			dataLinePanzerRed[3].end.x / 4, dataLinePanzerRed[3].end.y / 4, Color::Red, window);

	}
}
;


int main()// ������� ������� 
{

	double time1 = 0, time2 = 0; // ��������� ��� �������

	int countShotFocus = 0; // ������� �������� �������� ����� ��������� ���� ���� � ������
	srand(time(0));
	int selectMenu = 0;
	Clock clock;
	Font font;//����� 
	font.loadFromFile("times-new-roman.ttf");//�������� ������ ������ ���� ������
	Text text("", font, 20);//������� ������ �����. ���������� � ������ ����� ��p���, �����, ������ ������(� ��������);//��� ������ ����� (�� ������)

	font.loadFromFile("times-new-roman.ttf");//�������� ������ ������ ���� ������
	Text text1("", font, 40);//������� ������ �����. ���������� � ������ ����� ������, �����, ������ ������(� ��������);//��� ������ ����� (�� ������)
	Camera camera;

	camera.LinePanzerGreen();
	camera.LinePanzerRed();

	while (window.isOpen())
	{
		clock.restart();
		time1 = clock.getElapsedTime().asMicroseconds();// �������� ����� � ������ �����
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			if (event.type == Event::LostFocus)
			{
				pause = true;

			}
			if (event.type == Event::GainedFocus)
			{
				pause = false;
				NoShotFocus = true;
			}
		}
		if (startGame == false)
		{

			if (StartConnected == true && networkGame == false)// ���� ���� �������� � ���� ��������������������� ����
			{
				system("cls");//������� �������
							  /*std::cout << "s - server; c - client" << "\n";
							  std::cin >> mode;*/
				if (mode == 's')
				{
					std::cout << "Please wait for connection client";
					server.Listen();
				}
				if (mode == 'c')
				{
					std::cout << "new ip:";
					std::cin >> ip;
					/*std::cout << "new port:";
					std::cin >> port;*/
					client.Connect();
				}
				// networkGame==true ������������ � ��� ������ ����� �������������\����� ����������
				if (networkGame == true) // ���� ���������� ������������
				{

					if (mode == 's')
					{
						srand(10);
						walls.Placement(); // ���������� ���� 
					}
					//	for (int i=0;i<kolvoStens;i++)
					int k = 0;
					static bool viewSend = false;
					static bool viewRaceResponce = false;
					Data data;
					////////////////////// �������� ������ � ������ �������		
					if (mode == 's')// ����������� �� �������
					{
						while (viewSend == false)
						{
							if (viewRaceResponce == false)
							{
								server.Send(9, 0, 0, isView3D ? 1 : 0, 0);

							}
							if (viewRaceResponce == false)
							{
								data = server.Receive();
								if (data.key == 9 && data.tip == isView3D ? 1 : 0)
								{
									viewSend = true;
									viewRaceResponce = true;
								}
							}
						}
						if (viewSend == true)
						{
							while (k < amountWalls)
							{
								if (k == 0) 	walls.WallsSend(k);
								data = server.Receive();
								if (data.key == 8)
								{
									if (data.tip - 1 == k) k = data.tip;// ���� ������� �������� ����� � ��� ��������� � ��������
									walls.WallsSend(k);// �������� ����� � ������� �
								}
							}
						}
					}
					if (mode == 'c') // ����������� �� �������
					{
						while (viewSend == false)
						{
							data = client.Receive();
							if (data.key == 9)
							{
								if (data.tip == 0)
								{
									isView3D = false;
									mouseCapture = false;
									window.setMouseCursorVisible(true);
								}
								if (data.tip == 1)
								{
									isView3D = true;
									mouseCapture = true;
									window.setMouseCursorVisible(false);
								}
								client.Send(9, 0, 0, isView3D ? 1 : 0, 0);
								viewSend = true;
							}
						}
						if (viewSend == true)
						{
							while (k < amountWalls)
							{
								int flag = walls.WallsReceive();// ��������� ����� �������� ����� � ��������� �����
								if (flag == k)
								{
									k++;
									client.Send(8, 0, 0, k, 0);// �������� �������� ����� ������� � ������� ��������� �����
								}
							}
						}
					}
					camera.BlockToLines();
					startGame = true;
				}
			}
			selectMenu = mainmenu.Service(event);// �������� ���� 
			if (selectMenu == 1)
			{
				srand(10);
				walls.Placement();
				camera.BlockToLines();
				srand(time(0));
				startGame = true; // ���� ������ 1 ����� �������� ���� 
			}
			if (selectMenu == 2)// ���� ������ 2 ����� �������� ����
			{
				//network=true;

				StartConnected = true;
			}
		}
		if (startGame == true && networkGame == true)// ���� ���� ������� ���� ���������� 
		{
			if (networkGame == true)
			{
				if (mode == 's')
				{
					//client.receive();
					ServerRace();
					panzerBot.PanzerBotRace();
				}
				if (mode == 'c')
				{
					ClientRace();
					panzer.PanzerRace();
					ClientBonusRace();
				}
				burstes.Service();
				FlyBullets();
				NewCoordinateKill();
				GrabBonus();
				NewBonusGame();
			}
		}
		if (pause == false && startGame == true)// ���� ���� ���� � ������ � � ���� ����������
		{
			//panzer.Control(event);
			if (networkGame == true)
			{
				//	camera.LinePanzerGreen();
				//	camera.LinePanzerRed();

				if (mode == 's')
				{
					if (isView3D == false)panzer.Control(event); else panzer.Control3D(event);
					gameInterface.LoadData(panzer.Get_HP(), panzer.Get_Brony(),
						panzer.Get_Pylu(), panzer.Get_ChetAttack(), panzer.Get_TimeAttack());
					gameInterface.LoadDataBonus(panzer.speedTurn);
					//camera.Services(GREEN, window);
				}
				if (mode == 'c')
				{

					if (isView3D == false) panzerBot.Control(event); else panzerBot.Control3D(event);
					gameInterface.LoadData(panzerBot.Get_HP(), panzerBot.Get_Brony(),
						panzerBot.Get_Pylu(), panzerBot.Get_ChetAttack(), panzerBot.Get_TimeAttack());
					gameInterface.LoadDataBonus(panzerBot.speedTurn);
					//	camera.Services(RED, window);
				}
			}
			else
			{
				camera.LinePanzerGreen();
				camera.LinePanzerRed();
				camera.BonusToLine();
				camera.Services(GREEN, window);
				panzerBot.AutoControl();
				if (isView3D == true)
				{
					//Control3D event)
					panzer.Control3D(event);
				}
				else
				{
					panzer.Control(event);
				}
				gameInterface.LoadData(panzer.Get_HP(), panzer.Get_Brony(),
					panzer.Get_Pylu(), panzer.Get_ChetAttack(), panzer.Get_TimeAttack());
				gameInterface.LoadDataBonus(panzer.speedTurn);
			}

			if (networkGame == false && isView3D==true) camera.Services(GREEN, window);
			panzerBot.MovingToKeyboard();
			panzer.Servis();
			panzerBot.Servis();
			burstes.Service();
			FlyBullets();

			/*
			if (Keyboard::isKeyPressed(Keyboard::R))
			{
			bonuses.NewBonus(rand()%800,rand()%541,1);
			Sleep (10);
			}
			if (Keyboard::isKeyPressed(Keyboard::Z))
			{
			bonuses.NewBonus(rand()%800,rand()%541,1);
			Sleep (10);
			}
			*/
			if (Keyboard::isKeyPressed(Keyboard::M))
			{
				Vector2i mousePos = Mouse::getPosition(window);//�������� ���������� �������
				panzerBot.RegPointAim(mousePos.x + 20, mousePos.y + 20);
				sleep(milliseconds(10));
			}
			NewBonusGame();
			GrabBonus();                                        //  Get_ChetAttack()


			NewCoordinateKill();

			if (ClickMouseLeft(event) == false)NoShotFocus = false;
		}

		if (networkGame == true)
		{
			camera.LinePanzerGreen();
			camera.LinePanzerRed();
			camera.BonusToLine();
			if (mode == 's')
			{
				camera.Services(GREEN, window);
			}
			if (mode == 'c')
			{
				camera.Services(RED, window);
			}

		}
		//if (Keyboard::isKeyPressed(Keyboard::V))// // ��������� ������ ���� 2�/3�
		//{
		//	isView3D = !isView3D;
		//	while (Keyboard::isKeyPressed(Keyboard::V));
		//}
		window.clear();
		if (startGame == true)
		{
			if (isView3D == false)
			{
				panzer.Draw();
				panzerBot.Draw();
				for (int i = 0; i < amountBullet; i++)	bullets[i].Draw();
				walls.DrawWalls();

				bonuses.Draw();
				burstes.Draw();
			}
			else
			{
				if (isView3D)
				{
					camera.Draw(window);
					//camera.DrawLinePanzerREd(window);
				}
			}
			gameInterface.Draw();
			gameInterface.statistics.Draw(text);
			time2 = clock.getElapsedTime().asMicroseconds();
			fps.Server(time1, time2);
			fps.Draw(text);
			//		if (marshrut.being==true)marshrut.DrawPoisk(text);
		}
		else
		{
			mainmenu.Draw(text1);
		}
		window.display();

		do
		{
			time2 = clock.getElapsedTime().asMicroseconds();// �������� ����� � ����� �����
															//Sleep (1);
		} while (time2 - time1 < 15000);// ���� ������� �� ������� ����� ������� � ������ ����� 
										//������ ������������� ����� �� ��������� ����� ������ 
	}

	return 0;
}
